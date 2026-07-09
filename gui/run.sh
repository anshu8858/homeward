#!/usr/bin/env bash
set -e

# Builds and runs the GUI container via a plain `docker run --gpus all`
# instead of `docker compose up`.
#
# Why: docker-compose.yml's GPU device-reservation
# (deploy.resources.reservations.devices) was confirmed NOT to mount
# NVIDIA's GLX/Vulkan userspace libraries into the container -- Unreal
# Editor crashed on startup with VulkanRHI VK_ERROR_INCOMPATIBLE_DRIVER
# even though the libraries exist on the host. A plain
# `docker run --gpus all -e NVIDIA_DRIVER_CAPABILITIES=all` against the
# same base image, on the same host, DID mount them correctly (verified
# directly: libGLX_nvidia.so.0 present). Docker Compose here is v2
# (v5.0.0, not the legacy v1 that lacks this syntax outside Swarm) and
# the host's /etc/nvidia-container-runtime/config.toml has no capability
# restriction either -- so this is specifically a Compose-vs-plain-run
# GPU request translation quirk on this host/toolkit combination, not a
# driver, host-policy, or Compose-version problem. Rather than keep
# debugging that interaction blind, this script just uses the path that's
# actually confirmed to work.

cd "$(dirname "$0")"
PROJECT_DIR="$(cd .. && pwd)"
IMAGE_TAG="homeward-gui:dev-5.8"

docker build \
  --build-arg UE_IMAGE=ghcr.io/epicgames/unreal-engine:dev-5.8 \
  -t "$IMAGE_TAG" \
  .

docker run --rm -it \
  --gpus all \
  -e NVIDIA_DRIVER_CAPABILITIES=all \
  -e DISPLAY=:1 \
  -e UPROJECT_PATH=/workspace/Homeward.uproject \
  ${VNC_PASSWORD:+-e VNC_PASSWORD="$VNC_PASSWORD"} \
  -p 8444:8444 \
  -v "${PROJECT_DIR}:/workspace" \
  --shm-size 16g \
  "$IMAGE_TAG"
