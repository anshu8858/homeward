# Browser-accessible Unreal Editor for headless GPU servers (no local
# desktop/display). Layers XFCE + KasmVNC on top of Epic's official
# engine image so you can open Homeward.uproject in a real browser tab
# instead of needing a GPU on your own machine.
#
# Build with the SAME tag you already have locally, e.g.:
#   docker build --build-arg UE_IMAGE=ghcr.io/epicgames/unreal-engine:dev-5.8 -t homeward-gui .
ARG UE_IMAGE=ghcr.io/epicgames/unreal-engine:dev-5.8
FROM ${UE_IMAGE}

USER root

# XFCE for a lightweight desktop; curl/wget/ca-certificates to fetch the
# KasmVNC package matching this image's actual distro (not guessed).
RUN apt-get update && apt-get install -y --no-install-recommends \
    xfce4 \
    xfce4-terminal \
    dbus-x11 \
    curl \
    wget \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# KasmVNC ships one .deb per distro codename (jammy/focal/bookworm/etc),
# not a single universal package. Detect this image's actual codename at
# build time and pull the matching asset from the latest GitHub release,
# instead of hardcoding a version/URL that will silently go stale.
RUN . /etc/os-release \
    && CODENAME="${VERSION_CODENAME:?could not read VERSION_CODENAME from /etc/os-release}" \
    && echo "Detected distro codename: $CODENAME" \
    && DEB_URL="$(curl -fsSL https://api.github.com/repos/kasmtech/KasmVNC/releases/latest \
         | grep -o "https://[^\"]*${CODENAME}[^\"]*\.deb" | head -n1)" \
    && if [ -z "$DEB_URL" ]; then \
         echo "ERROR: no KasmVNC .deb found for codename '$CODENAME' in the latest release." >&2; \
         echo "Check https://github.com/kasmtech/KasmVNC/releases and adjust this Dockerfile." >&2; \
         exit 1; \
       fi \
    && echo "Downloading $DEB_URL" \
    && curl -fsSL -o /tmp/kasmvncserver.deb "$DEB_URL" \
    && apt-get update && apt-get install -y /tmp/kasmvncserver.deb \
    && rm -f /tmp/kasmvncserver.deb && rm -rf /var/lib/apt/lists/*

# Same non-root user the engine image's own binaries are owned by (see
# build_docker.sh at the project root -- UnrealEditor/RunUAT.sh are only
# executable by their real owner inside the image, and the cook
# commandlet separately hard-refuses to run as root at all). Editor GUI
# sessions follow the same non-root convention for consistency.
RUN adduser ue4 ssl-cert || true

COPY start.sh /start.sh
RUN chmod +x /start.sh

USER ue4
WORKDIR /workspace
ENV DISPLAY=:1
ENV HOME=/home/ue4

ENTRYPOINT ["/start.sh"]
