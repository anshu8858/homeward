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
# KasmVNC package matching this image's actual distro (not guessed);
# vulkan-tools/mesa-utils so `vulkaninfo`/`glxinfo` are available for
# diagnosing GPU/rendering issues without a rebuild-then-manual-install
# round trip (this is exactly how the Vulkan/RHI crash on this stack
# actually got diagnosed).
RUN apt-get update && apt-get install -y --no-install-recommends \
    xfce4 \
    xfce4-terminal \
    dbus-x11 \
    curl \
    wget \
    ca-certificates \
    vulkan-tools \
    mesa-utils \
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

# VirtualGL: Xvnc (KasmVNC's X server) is software-only and cannot create
# a Vulkan/GPU-capable window surface -- confirmed directly on real
# hardware (vulkaninfo's X_CreateWindow probe against Xvnc fails with
# BadMatch, even though the GPU/driver/ICD are all otherwise fully
# working: nvidia-smi and the Vulkan ICD registration both check out
# inside this exact container). VirtualGL redirects an app's GL/Vulkan
# rendering to the real GPU and reads the result back into whatever X
# display it's pointed at, which is how apps needing real GPU rendering
# can still show up inside a VNC session. Unlike KasmVNC, VirtualGL ships
# ONE universal .deb per arch (no per-distro-codename variant), so this
# just resolves the latest version tag and downloads it directly --
# still not hardcoding a version that will go stale.
RUN VGL_VERSION="$(curl -fsSL https://api.github.com/repos/VirtualGL/virtualgl/releases/latest \
         | grep -o '"tag_name": *"[^"]*"' | head -n1 | grep -o '[0-9][0-9.]*')" \
    && if [ -z "$VGL_VERSION" ]; then \
         echo "ERROR: could not resolve the latest VirtualGL version from the GitHub API." >&2; \
         exit 1; \
       fi \
    && VGL_URL="https://github.com/VirtualGL/virtualgl/releases/download/${VGL_VERSION}/virtualgl_${VGL_VERSION}_amd64.deb" \
    && echo "Downloading $VGL_URL" \
    && curl -fsSL -o /tmp/virtualgl.deb "$VGL_URL" \
    && apt-get update && apt-get install -y /tmp/virtualgl.deb \
    && rm -f /tmp/virtualgl.deb && rm -rf /var/lib/apt/lists/*

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
