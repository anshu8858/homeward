#!/usr/bin/env bash
set -e

# Epic Games Unreal Engine Image (Requires GitHub Container Registry Auth)
# Note: Adjust version tag to match your target (using 5.4/5.5 standard tag format)
UE_IMAGE="ghcr.io/epicgames/unreal-engine:dev-5.8" 

if ! command -v docker &> /dev/null; then
    echo "Error: Docker could not be found. Please install Docker."
    exit 1
fi

PROJECT_DIR=$(pwd)

echo "Building Homeward using Unreal Engine Docker Image: $UE_IMAGE"
echo "Note: You must have run 'docker login ghcr.io' with an Epic Games linked GitHub account."

# The command to execute inside the Unreal Engine container
BUILD_COMMAND="/home/ue4/UnrealEngine/Engine/Build/BatchFiles/RunUAT.sh BuildCookRun \
  -project=\"/project/Homeward.uproject\" \
  -noP4 -platform=Linux -clientconfig=Shipping \
  -serverconfig=Shipping -cook -allmaps -build -stage -pak -archive \
  -archivedirectory=\"/project/ArchivedBuilds\""

# Pre-create build directories and ensure they are writable by the container's default non-root user (ue4)
mkdir -p Saved Intermediate Binaries Build DerivedDataCache Config Content/Developers ArchivedBuilds
chmod -R 777 Saved Intermediate Binaries Build DerivedDataCache Config Content/Developers ArchivedBuilds

# Run as the UID/GID that OWNS the project directory, not whoever invoked
# this script. Using "$(id -u):$(id -g)" directly is wrong if this script
# is run via sudo/as root (id would report 0:0, and Unreal's cook
# commandlet hard-refuses to run as root: "Refusing to run with the root
# privileges" -- it's a deliberate safety check, not a bug). The
# directory owner is always the right, non-root UID to run as regardless
# of how this script itself was invoked.
RUN_UID_GID="$(stat -c '%u:%g' "$PROJECT_DIR")"
if [ "${RUN_UID_GID%%:*}" = "0" ]; then
    echo "Error: $PROJECT_DIR is owned by root. Unreal's cook step refuses to run as root."
    echo "Fix ownership first, e.g.: sudo chown -R \$SUDO_USER:\$SUDO_USER \"$PROJECT_DIR\""
    exit 1
fi

# Without matching this UID, every file the container writes into the
# bind-mounted /project is owned by whatever UID the image defaults to
# (commonly a baked-in "ue4" at 1000); if that doesn't match the host
# owner, every chmod/write UAT does during archiving fails with
# "Operation not permitted" -- even though build/cook/stage (which
# happen entirely inside the container's own filesystem) succeed fine.
docker run --rm -it \
  -v "${PROJECT_DIR}:/project" \
  -w "/project" \
  --user "${RUN_UID_GID}" \
  "${UE_IMAGE}" \
  bash -c "${BUILD_COMMAND}"

