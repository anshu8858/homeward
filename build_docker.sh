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

# Pre-create build directories and ensure they are writable regardless of
# which UID ends up owning them below.
mkdir -p Saved Intermediate Binaries Build DerivedDataCache Config Content/Developers ArchivedBuilds
chmod -R 777 Saved Intermediate Binaries Build DerivedDataCache Config Content/Developers ArchivedBuilds

# Unreal's cook commandlet hard-refuses to run as root ("Refusing to run
# with the root privileges" -- a deliberate safety check). This must work
# whether invoked as a normal user, via sudo, or as bare root, on any
# machine, without manual chown steps -- so it self-heals rather than
# erroring out:
#   1. Not root already -> use our own UID/GID (already non-root, and
#      already owns whatever it created).
#   2. Root via `sudo` (SUDO_USER set) -> use that real user's UID/GID.
#   3. Root some other way, but the project directory is already
#      non-root-owned -> use that owner as-is.
#   4. Root with a root-owned project directory (e.g. cloned as root) ->
#      fall back to a fixed non-root UID/GID (1000:1000) and chown the
#      directory to it. 1000 doesn't need to correspond to a real user on
#      this host or in the image; Docker/Unreal only care that it isn't 0.
if [ "$(id -u)" != "0" ]; then
    RUN_UID_GID="$(id -u):$(id -g)"
elif [ -n "${SUDO_USER:-}" ]; then
    RUN_UID_GID="$(id -u "$SUDO_USER"):$(id -g "$SUDO_USER")"
else
    CURRENT_OWNER="$(stat -c '%u:%g' "$PROJECT_DIR")"
    if [ "${CURRENT_OWNER%%:*}" != "0" ]; then
        RUN_UID_GID="$CURRENT_OWNER"
    else
        RUN_UID_GID="1000:1000"
    fi
fi

if [ "${RUN_UID_GID%%:*}" = "0" ]; then
    # Should be unreachable given the logic above, but never hand Unreal a
    # root UID -- fail loudly instead of letting cook fail obscurely.
    echo "Error: could not determine a non-root UID/GID to run as."
    exit 1
fi

if [ "$(stat -c '%u:%g' "$PROJECT_DIR")" != "$RUN_UID_GID" ] && [ "$(id -u)" = "0" ]; then
    echo "Fixing ownership of $PROJECT_DIR to $RUN_UID_GID..."
    chown -R "$RUN_UID_GID" "$PROJECT_DIR"
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

