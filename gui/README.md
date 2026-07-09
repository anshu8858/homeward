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
docker compose build
docker compose up
```

Watch the logs for the generated KasmVNC password and the actual URL to use (`vncserver` prints its own real port on every start -- trust that over any port number written here):

```bash
docker compose logs -f
```

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
