#!/bin/bash
set -e

# Entrypoint: brings up KasmVNC + XFCE, then launches UnrealEditor on the
# mounted project. Runs as the non-root ue4 user (set via Dockerfile's
# USER directive) -- matches the same reasoning as build_docker.sh at the
# project root: don't run engine processes as root.

UPROJECT_PATH="${UPROJECT_PATH:-/workspace/Homeward.uproject}"
VNC_GEOMETRY="${VNC_GEOMETRY:-1920x1080}"
VNC_PASSWORD="${VNC_PASSWORD:-$(tr -dc 'A-Za-z0-9' </dev/urandom | head -c 16)}"

mkdir -p "$HOME/.vnc"

# Disable KasmVNC's clipboard/DLP sync entirely. Its clipboard handling
# (the "Adding DLP binary mime type..." lines in the logs) segfaults
# Xvnc itself -- not just the app -- on a PRIMARY-selection request:
# rfb::utf8ToLatin1 -> vncGetScreenImage -> crash, killing the whole X
# server and therefore the entire desktop/session. Reproduced twice with
# an identical stack trace. Write this BEFORE vncserver runs -- it only
# generates a default kasmvnc.yaml if one doesn't already exist.
cat > "$HOME/.vnc/kasmvnc.yaml" <<'EOF'
data_loss_prevention:
  clipboard:
    server_to_client:
      enabled: false
    client_to_server:
      enabled: false
EOF

# Non-interactive password setup. KasmVNC's exact vncpasswd flags have
# shifted across releases -- if this errors on your image's KasmVNC
# version, run `vncpasswd --help` inside the container and adjust.
printf '%s\n%s\n' "$VNC_PASSWORD" "$VNC_PASSWORD" | vncpasswd -u ue4 -w -r

echo "=================================================================="
echo " KasmVNC password: $VNC_PASSWORD"
echo " (also settable up front via the VNC_PASSWORD env var)"
echo "=================================================================="

# -select-de xfce skips the interactive desktop-environment prompt AND
# makes vncserver's own generated ~/.vnc/xstartup script launch
# xfce4-session itself -- do NOT also call startxfce4 here. Running both
# races two independent xfce4-session instances for window-manager
# ownership on the same display, which intermittently fails with
# "Another Window Manager (unknown) is already running" / xfwm4 exiting
# (found the hard way -- see git history on this file).
vncserver "$DISPLAY" \
    -geometry "$VNC_GEOMETRY" \
    -depth 24 \
    -select-de xfce \
    -disableBasicAuth

sleep 3

if [ -f "$UPROJECT_PATH" ]; then
    echo "Launching UnrealEditor on $UPROJECT_PATH (via vglrun -d egl) ..."
    # Xvnc (KasmVNC's X server) is software-only and can't offer a
    # GPU-capable window surface -- confirmed directly (vulkaninfo's
    # window-creation probe against it fails with X_CreateWindow/BadMatch
    # even though the GPU/driver/Vulkan ICD are all otherwise fully
    # working). vglrun -d egl redirects the actual rendering to the real
    # GPU via EGL (no second X server needed, unlike VirtualGL's classic
    # setup) and reads the result back into this ($DISPLAY) X session.
    vglrun -d egl /home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealEditor "$UPROJECT_PATH" &
else
    echo "WARNING: $UPROJECT_PATH not found -- mount your project to /workspace." >&2
    echo "Desktop is still up; open a terminal in XFCE and launch the editor manually." >&2
fi

# Keep the container alive and let Ctrl-C / `docker stop` shut it down
# cleanly instead of the script just exiting under the running services.
tail -f "$HOME"/.vnc/*.log 2>/dev/null || sleep infinity
