# Browser-based Unreal Editor (headless GPU server, no local desktop)

For running the Editor on a remote NVIDIA GPU server that has no desktop
environment of its own -- e.g. exactly the situation this project's main
dev machine is in (no local GPU). This layers XFCE + KasmVNC on top of
the same `ghcr.io/epicgames/unreal-engine` image `build_docker.sh` (at
the project root) already uses for headless packaging, so you open a
real Unreal Editor window in a browser tab instead.

```
Your browser (any machine)
        │ HTTPS :6901
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

Watch the logs for the generated KasmVNC password (or set your own via `VNC_PASSWORD` in `docker-compose.yml` before starting):

```bash
docker compose logs -f | grep "KasmVNC password"
```

Then open, from any browser, on any machine that can reach the server:

```
https://SERVER_IP:6901
```

Accept the self-signed cert warning, log in with the printed password, and you'll get an XFCE desktop with `UnrealEditor` already open on `Homeward.uproject`.

## Known rough edges

- **`vncpasswd`/`vncserver` flag names** (`gui/start.sh`) were written against KasmVNC's documented interface but not verified by actually running this stack -- if either command errors on the specific KasmVNC version the Dockerfile resolves for your image's distro, run `vncpasswd --help` / `vncserver --help` inside the container and adjust `start.sh`.
- **KasmVNC package selection** happens at build time by matching the base image's `/etc/os-release` codename against KasmVNC's latest GitHub release assets -- if Epic ever rebases the `dev-5.8` image onto a distro codename KasmVNC hasn't published a `.deb` for yet, the build fails loudly at that step (by design) rather than silently installing the wrong package.
- If `UnrealEditor` doesn't appear after a minute, open a terminal from the XFCE desktop and launch it manually: `/home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealEditor /workspace/Homeward.uproject`.
