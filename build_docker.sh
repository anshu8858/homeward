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
# with the root privileges" -- a deliberate safety check), so this can't
# just run as whoever invoked the script. But it also can't invent an
# arbitrary non-root UID (e.g. a guessed 1000:1000): the image's own
# baked-in engine binaries (RunUAT.sh etc, under /home/ue4/UnrealEngine)
# are only readable/executable by their real owner, the "ue4" user inside
# the image -- an unrelated UID that happens to not be 0 gets a plain
# "Permission denied" trying to even run RunUAT.sh, before Unreal itself
# is involved at all.
#
# So: ask the image what ue4's real UID/GID are, and make the mounted
# project directory match that -- one correct UID for both problems
# (executing the engine's own files, and writing into /project), instead
# of guessing.
UE4_ID_LINE="$(docker run --rm "${UE_IMAGE}" id ue4)"
UE4_UID="$(echo "$UE4_ID_LINE" | sed -n 's/uid=\([0-9]*\).*/\1/p')"
UE4_GID="$(echo "$UE4_ID_LINE" | sed -n 's/.*gid=\([0-9]*\).*/\1/p')"
if [ -z "$UE4_UID" ] || [ -z "$UE4_GID" ]; then
    echo "Error: could not determine the ue4 user's UID/GID from ${UE_IMAGE}."
    echo "Got: $UE4_ID_LINE"
    exit 1
fi
RUN_UID_GID="${UE4_UID}:${UE4_GID}"

CURRENT_OWNER="$(stat -c '%u:%g' "$PROJECT_DIR")"
if [ "$CURRENT_OWNER" != "$RUN_UID_GID" ]; then
    echo "Fixing ownership of $PROJECT_DIR to match the image's ue4 user ($RUN_UID_GID)..."
    if [ "$(id -u)" = "0" ]; then
        chown -R "$RUN_UID_GID" "$PROJECT_DIR"
    else
        sudo chown -R "$RUN_UID_GID" "$PROJECT_DIR"
    fi
fi

docker run --rm -it \
  -v "${PROJECT_DIR}:/project" \
  -w "/project" \
  --user "${RUN_UID_GID}" \
  "${UE_IMAGE}" \
  bash -c "${BUILD_COMMAND}"

