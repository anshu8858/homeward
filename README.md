# Homeward 📦

**Homeward** is a 3D narrative packing-puzzle game built in Unreal Engine 5.8 for Android. It combines the addictive, flow-inducing mechanics of a block-packing puzzle with a deeply emotional, wordless narrative about moving homes and the memories attached to our belongings.

## 🏗️ Architecture & Philosophy
- **Engine:** Unreal Engine 5.8 (C++ Core, Blueprints for UI/Glue).
- **Target:** Android API 24+, Arm64-v8a, Portrait orientation, APK ≤ 100MB.
- **Visuals:** Flat-shaded, low-poly style relying on unlit materials and 1D procedural gradient palettes (e.g., *Grandmother's House* theme).
- **Audio:** Custom LMMS felt-piano loops heavily optimized for mobile phone speakers (mono-summed, 150Hz high-pass).
- **Input:** Single-verb interaction. Drag to move, tap to yaw-rotate. No pitch/roll, no multi-touch, static orthographic camera.

## 🚀 Getting Started

### Prerequisites
- Unreal Engine 5.8
- Android SDK, NDK, and JDK 17 (for Android packaging)
- Python 3.x (for offline tools)

### Asset Generation
Before opening the project, generate the greybox meshes and placeholder audio files:
```bash
python3 scripts/generate_greybox_meshes.py
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

### Offline Level Verification
Levels can be verified mathematically before being added to the game using the exact-cover Dancing Links (DLX) solver:
```bash
python3 scripts/dlx_solver.py
```

## 📜 License
*All code and assets are proprietary unless otherwise specified (e.g., CC0 placeholders).*
