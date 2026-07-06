#!/bin/bash
set -e

# Path to Unreal Engine installation - update this path based on where UE 5.8 is installed on your Linux machine
UE_DIR=${UE_DIR:-"/opt/UnrealEngine"}
UAT_SCRIPT="$UE_DIR/Engine/Build/BatchFiles/RunUAT.sh"

PROJECT_FILE="$(pwd)/Homeward.uproject"
ARCHIVE_DIR="$(pwd)/Builds/Android"

if [ ! -f "$UAT_SCRIPT" ]; then
    echo "Error: RunUAT.sh not found at $UAT_SCRIPT."
    echo "Please set the UE_DIR environment variable to your Unreal Engine installation."
    exit 1
fi

echo "Building Homeward for Android (Headless)..."

# Build, Cook, and Package for Android
"$UAT_SCRIPT" BuildCookRun \
    -project="$PROJECT_FILE" \
    -noP4 \
    -platform=Android \
    -clientconfig=Development \
    -serverconfig=Development \
    -cook \
    -allmaps \
    -build \
    -stage \
    -pak \
    -archive \
    -archivedirectory="$ARCHIVE_DIR" \
    -utf8output

echo "Build process completed. Check $ARCHIVE_DIR for the generated APK."
