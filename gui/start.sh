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
    echo "Launching UnrealEditor on $UPROJECT_PATH ..."
    /home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealEditor "$UPROJECT_PATH" &
else
    echo "WARNING: $UPROJECT_PATH not found -- mount your project to /workspace." >&2
    echo "Desktop is still up; open a terminal in XFCE and launch the editor manually." >&2
fi

# Keep the container alive and let Ctrl-C / `docker stop` shut it down
# cleanly instead of the script just exiting under the running services.
tail -f "$HOME"/.vnc/*.log 2>/dev/null || sleep infinity
