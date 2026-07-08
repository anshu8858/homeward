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
  -noP4 -platform=Android -clientconfig=Shipping \
  -serverconfig=Shipping -cook -allmaps -build -stage -pak -archive \
  -archivedirectory=\"/project/Saved/StagedBuilds\""

# Run the docker container, mounting the local project into /project
docker run --rm -it \
  --user root \
  -v "${PROJECT_DIR}:/project" \
  -w "/project" \
  "${UE_IMAGE}" \
  bash -c "${BUILD_COMMAND}"

