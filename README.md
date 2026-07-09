# Homeward 📦

**Homeward** is a 3D narrative packing-puzzle game built in Unreal Engine 5.8 for Android. It combines the addictive, flow-inducing mechanics of a block-packing puzzle with a deeply emotional, wordless narrative about moving homes and the memories attached to our belongings.

## 🏗️ Architecture & Philosophy
- **Engine:** Unreal Engine 5.8 (C++ Core, Blueprints for UI/Glue).
- **Target:** Android API 24+, Arm64-v8a, Portrait orientation, APK ≤ 100MB.
- **Visuals:** Flat-shaded, low-poly style relying on unlit materials and 1D procedural gradient palettes (e.g., *Grandmother's House* theme).
- **Audio:** Custom LMMS felt-piano loops heavily optimized for mobile phone speakers (mono-summed, 150Hz high-pass).
- **Input:** Single-verb interaction. Drag to move, tap to yaw-rotate. No pitch/roll, no multi-touch, static orthographic camera.

## 📦 Current Implementation Status

The gameplay logic layer is implemented and unit-tested in C++ (see `docs/PLAN.md` for the full session-by-session history):

- **`FGridModel`** (`Source/Homeward/Logic/GridModel.*`): active-layer packing order, Fragile (top layer only), Heavy (bottom layer only), Tall (reserves the cell above).
- **`APiece` / `APackingBox`** (`Logic/Piece.*`, `Logic/PackingBox.*`): the in-world item actor and the grid↔world coordinate/placement owner.
- **`UDragController`** (`Logic/DragController.*`): raycasts touch input, disambiguates tap-vs-drag, drives placement through `APackingBox`.
- **`AHomewardGameMode`**: level-flow glue -- connects `APackingBox::IsLevelComplete()` to the difficulty manager's gentle-variant check, chapter/level advancement, and daily-challenge completion.
- **`USettingsManager` / `UDynamicDifficultyManager` / `UMemoryOfTheDayManager`**: persistence, honest DDA, and the daily-streak system.
- **Content**: `scripts/level_data.py` has all 6 chapters / 28 levels authored; `scripts/generate_greybox_meshes.py` produces all 28 placeholder meshes.

**Not yet done, and needs the Editor (GPU) to do:** binding the above to actual `.uasset` `PieceDef`/`LevelDef`/`ChapterDef` instances, materials, camera rig, UMG screens, and vignette `LevelSequence` assets.

## 🚀 Getting Started

### Prerequisites
- Unreal Engine 5.8
- Android SDK, NDK, and JDK 17 (for Android packaging)
- Python 3.x (for offline tools)

### Asset Generation
Before opening the project, generate the greybox meshes and placeholder audio files:
```bash
python3 scripts/generate_greybox_meshes.py   # 28 meshes: all polyomino shapes + hero/story props + Tall props
python3 scripts/generate_audio_placeholders.py
python3 scripts/generate_palette.py
```

### Headless Building (Local Engine)
If you have Unreal Engine installed natively on your Linux machine, you can compile the C++ logic, cook the content, and package the Android APK without opening the Unreal Editor by running:
```bash
UE_DIR=/path/to/UnrealEngine ./build_headless.sh
```

### Docker Building (No Local Engine Required)
If you do not have Unreal Engine installed on your machine, you can build the APK using Epic Games' official Docker containers.
1. Link your Epic Games account to your GitHub account.
2. Generate a GitHub Personal Access Token (PAT) with `read:packages` permissions.
3. Authenticate Docker: `docker login ghcr.io -u YOUR_GITHUB_USERNAME`
4. Run the wrapper script to mount your project and compile inside the container:
```bash
./build_docker.sh
```
`build_docker.sh` self-heals ownership: it queries the image for the real `ue4` UID/GID and `chown`s the project directory to match, so it works whether you run it as yourself, via `sudo`, or as root -- no manual `chown` needed.

For just a fast compile check (no cook/package, catches C++ errors in minutes instead of tens of minutes):
```bash
docker run --rm -it -v "$(pwd):/project" -w /project ghcr.io/epicgames/unreal-engine:dev-5.8 \
  bash -c '/home/ue4/UnrealEngine/Engine/Build/BatchFiles/Linux/Build.sh HomewardEditor Linux Development -project="/project/Homeward.uproject"'
```

### Offline Level Verification
`scripts/dlx_solver.py` implements exact-cover (Dancing Links) validation, matching `FGridModel::CanPlacePiece`'s Fragile/Heavy/Tall rules exactly. `scripts/level_data.py` has all 28 levels authored against that solver's schema, with hand-verified tilings documented inline -- but **the two files are not yet wired together**: running `dlx_solver.py` directly only exercises its own small built-in example, not the real level data. Until a driver script exists to run every level in `level_data.py`'s `CHAPTERS` through the solver, treat the level content as hand-checked but solver-unverified -- especially Chapter 5 (Attic), which combines Fragile+Heavy+Tall in single levels.

## 📜 License
*All code and assets are proprietary unless otherwise specified (e.g., CC0 placeholders).*
