# Browser-based Unreal Editor (headless GPU server, no local desktop)

For running the Editor on a remote NVIDIA GPU server that has no desktop
environment of its own -- e.g. exactly the situation this project's main
dev machine is in (no local GPU). This layers XFCE + KasmVNC on top of
the same `ghcr.io/epicgames/unreal-engine` image `build_docker.sh` (at
the project root) already uses for headless packaging, so you open a
real Unreal Editor window in a browser tab instead.

```
Your browser (any machine)
        │ HTTPS :8444
        ▼
KasmVNC ── XFCE desktop ── UnrealEditor ── Epic's engine image ── NVIDIA GPU
```

## Prerequisites (on the GPU server)

- Docker + the [NVIDIA Container Toolkit](https://github.com/NVIDIA/nvidia-container-toolkit) (`docker run --rm --gpus all ghcr.io/epicgames/unreal-engine:dev-5.8 nvidia-smi` should print your GPU).
- Already authenticated to `ghcr.io` (`docker login ghcr.io`, per the project root README) -- you said you already have the `dev-5.8` image pulled, so this is likely done.

## Ownership, before you start (same issue `build_docker.sh` self-heals for packaging)

Unreal's own binaries inside the image are only executable by their real owner (`ue4`), and the Editor writes `Saved/`/`Intermediate/`/`DerivedDataCache/` into the mounted project as that same user. If the project directory isn't already owned by that UID, fix it once:

```bash
cd .. # project root
UE4_UID_GID="$(docker run --rm ghcr.io/epicgames/unreal-engine:dev-5.8 id -u ue4):$(docker run --rm ghcr.io/epicgames/unreal-engine:dev-5.8 id -g ue4)"
sudo chown -R "$UE4_UID_GID" .
```

(This is the same UID `build_docker.sh` already discovers and chowns to for you when packaging -- if you've run that script here before, this is likely already correct.)

## Build and run

```bash
cd gui
./run.sh
```

`run.sh` builds the image and runs it with a plain `docker run --gpus all` -- **use this, not `docker compose up`**: on real hardware, `docker-compose.yml`'s GPU device-reservation syntax was confirmed to not mount NVIDIA's GLX/Vulkan libraries into the container (Editor crashed with `VK_ERROR_INCOMPATIBLE_DRIVER`), while the exact same host's plain `docker run --gpus all` mounted them correctly. `docker-compose.yml` is kept only for `docker compose build` / as a config reference.

Watch the logs for the generated KasmVNC password and the actual URL to use (`vncserver` prints its own real port on every start -- trust that over any port number written here). `run.sh` runs in the foreground, so the logs are just in your terminal; if you instead run it detached, use `docker logs -f <container>`.

Look for lines like:
```
 KasmVNC password: <16 random chars>
Paste this url in your browser:
https://<container-internal-ip>:8444
```

Take the port from that second line (8444 as of this writing, but confirm it -- it comes from KasmVNC's own default, not something this project controls), and open, from any browser on any machine that can reach the server:

```
https://SERVER_IP:8444
```

(substitute your real server's IP/hostname for the container-internal IP KasmVNC prints). Accept the self-signed cert warning, log in with the printed password, and you'll get an XFCE desktop with `UnrealEditor` already open on `Homeward.uproject`.

## Known rough edges

- **Port number.** `docker-compose.yml` maps 8444, matching what KasmVNC printed when this was last run -- but that's KasmVNC's own default, not something pinned by this project's config, so re-check the "Paste this url" line in your own logs rather than trusting 8444 blindly.
- **Don't add `startxfce4` back into `start.sh`.** `-select-de xfce` already makes vncserver's generated `~/.vnc/xstartup` launch `xfce4-session` itself; adding a second launcher races two sessions for window-manager ownership and intermittently fails with `Another Window Manager (unknown) is already running` / xfwm4 refusing to start. Found this by actually running the stack -- see git history on `start.sh`.
- If `UnrealEditor` doesn't appear after a minute, open a terminal from the XFCE desktop and launch it manually: `/home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealEditor /workspace/Homeward.uproject`.

## If the Editor crashes on startup with a Vulkan/RHI error

Symptom: `UnrealEditor` loads plugins/config for a while then dies with a `VulkanRHI::VerifyVulkanResult` crash (signal 11), or `vulkaninfo`/`glxinfo` inside the container show `ERROR_INCOMPATIBLE_DRIVER` / `llvmpipe` (software rendering) instead of your real GPU. This means the container never got real GPU rendering access -- the Editor was trying to initialize Vulkan against a software-only display.

1. **Confirm the host actually has GL/Vulkan userspace libraries**, not just the compute driver (`nvidia-smi` working is NOT enough -- that only proves the compute/NVML driver is there; GLX/EGL/Vulkan are separate):
   ```bash
   ls -la /usr/lib/x86_64-linux-gnu/libGLX_nvidia.so.0 /usr/lib/x86_64-linux-gnu/libEGL_nvidia.so.0
   dpkg -l | grep -i nvidia-headless   # a "headless" driver metapackage deliberately excludes these
   ```
   If missing, install the matching `libnvidia-gl-<version>[-server]` package for your driver's exact version series (e.g. `libnvidia-gl-570-server` for driver `570.x`) and restart Docker's GPU containers.

2. **Confirm the toolkit is actually mounting them into a container**, in isolation from Compose:
   ```bash
   docker run --rm --gpus all -e NVIDIA_DRIVER_CAPABILITIES=all ghcr.io/epicgames/unreal-engine:dev-5.8 \
     bash -c "ls -la /usr/lib/x86_64-linux-gnu/libGLX_nvidia.so.0"
   ```
   If this works, use `./run.sh` instead of `docker compose up` -- that's exactly the situation this project hit: `docker compose version` reported v2 (`v5.0.0`, not the legacy v1 that lacks GPU-reservation support outside Swarm), `/etc/nvidia-container-runtime/config.toml` had no capability restriction, and the plain `docker run --gpus all` above still worked while `docker compose up` with the same env vars did not. That's a Compose-vs-plain-run GPU request translation quirk on some host/toolkit combinations -- `run.sh` sidesteps it entirely by using the confirmed-working invocation.

3. If even the plain `docker run --gpus all` test above fails to find the library, check `/etc/nvidia-container-runtime/config.toml`'s `supported-driver-capabilities` line -- if it's missing `graphics`/`display`, that's a host-level policy restriction (common on managed/shared GPU cloud, some providers lock containers to compute-only for isolation) and not something fixable from this repo.

4. If steps 1-3 all check out (library present, mounted, no policy restriction) and the Editor *still* crashes with `VK_ERROR_INCOMPATIBLE_DRIVER`, confirm it with `vulkaninfo --summary` run **inside the container**: if it fails with an X11 error (`X_CreateWindow` / `BadMatch`) rather than a missing-library error, the GPU/driver themselves are fine -- the problem is that Xvnc (KasmVNC's X server) is software-only and can't offer a GPU-capable window surface at all. This is a fundamentally different problem from 1-3 and no amount of library/capability fixing solves it. See "GPU rendering vs. Xvnc" below.

## GPU rendering vs. Xvnc (VirtualGL)

Confirmed directly on real hardware: `nvidia-smi` and the Vulkan ICD registration both work fine *inside* the container, but `vulkaninfo`'s window-creation probe against Xvnc fails with `X_CreateWindow ... BadMatch`. Xvnc is a software-only X server -- it has no GPU-capable visual to offer, so any app requesting real GPU rendering (UnrealEditor included) fails at that point regardless of how correctly the GPU/driver/capabilities are otherwise configured.

The Dockerfile now installs [VirtualGL](https://virtualgl.org/), which redirects an app's GL/Vulkan rendering to the real GPU and reads the result back into whichever X display it's pointed at -- the standard fix for "GPU app inside a VNC session." **Wiring it into `start.sh`'s actual Editor launch is not done yet** -- rather than guess `vglrun`'s exact flags from documentation (a mistake already made once this session with KasmVNC's flags), rebuild and run `vglrun --help` inside the container first to get the real, version-correct syntax, then update `start.sh`'s launch line to use it.

Note this project's specific constraint: the GPU server here is shared/multi-tenant (other tenants' ML workloads were visible in `nvidia-smi`'s process list). VirtualGL's classic setup needs a *second* Xorg bound directly to the GPU with exclusive display/VT access -- invasive on a shared box. Its EGL-only backend avoids that (no second X server needed) and is the one to use here if `vglrun --help` shows it, though VirtualGL's own docs note it "generally works fine... but some esoteric application behaviors still trip it up" -- Unreal Editor is a large, complex application, so this may or may not actually work. If it doesn't, the realistic alternatives are a GPU instance actually provisioned for remote-desktop/streaming use (not a compute-only shared box), or doing the M2 Editor-bound work (binding DataAssets, materials, meshes) somewhere with a real GPU display attached.
