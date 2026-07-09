# HOMEWARD — Android-First 3D Narrative Packing-Puzzle: Build Plan

## Context

User wants an Android-first game (cross-platform later) with broad appeal, a strong story, addictive-but-ethical design grounded in human psychology, easy one-hand play, OSS/self-made assets, buildable on a CPU-only Linux machine (GPU later). Two research workflows ran (market/psychology/story/engine/assets, then mechanics/architecture/art-audio design). User picked concept **#1 "Homeward"** and requested **3D**.

The game: wordless narrative 3D block-packing puzzle. A grandmother packs her lifelong home to move to her granddaughter's city. Each chapter = one room; puzzle pieces = her belongings; completing a room reveals a silent memory vignette. Arc: reluctance → grief → acceptance → reunion. Block-puzzle is the #1 download genre (Block Blast: 368M installs 2025) with the best retention class; the story wedge is the differentiator no competitor has (Monument Valley playbook — organic/press instead of UA spend).

## Locked decisions

- **Engine:** Unreal Engine **5.8** (latest), C++ for core logic and Blueprints for high-level UI/glue. Target Mobile/Android scalable settings.
- **Target:** Android API 24+, arm64-v8a only, portrait, one-hand, **APK ≤ 100 MB**, 60 fps on mid-range devices (graceful 30 fps floor).
- **Camera:** fixed orthographic, yaw 45° / pitch −32°, never moves during play (Monument Valley framing). No camera control, no multi-touch.
- **Input:** ONE verb (drag) + ONE modifier (tap = 90° **yaw-only** rotate, 4-state cycle). Pitch/roll rotation permanently banned. Tap-vs-drag threshold 12 dp / 150 ms; snap magnetism 0.6 cell; dragged piece offset +1.5 cell vertical so thumb never occludes ghost.
- **Grid:** layered 2.5D — true 3D box (`FIntVector(W, layers, D)`) packed one horizontal layer at a time; player thinks in 2D per layer; layer N+1 unlocks at 100% fill. Sizes 3×1×3 → 5×3×4. Every level exact-fit, solver-verified (DLX) offline.
- **No fail state:** no timers/lives/energy; unlimited undo; long-press 600 ms = shake-the-box reset (zero penalty).
- **Scope v1:** 6 chapters (Kitchen, Living Room, Bedroom, Child's Room, Attic, Epilogue-unpacking), 5 levels each ≈ 28 levels, ~60–75 min campaign. (Study + Garden Shed rooms = post-launch pack.)
- **Story delivery:** zero text except numerals + settings. Environmental storytelling: last-packed hero item per level, lighting temperature per act, 1 pantomime vignette per chapter (in-engine 3D dioramas reusing game assets — silhouette figures + warm backlight, 8–12 s, skippable after 3 s), recurring teapot/music-box motifs. Epilogue reverses the verb: unpack into the new apartment.
- **Ethical psychology:** flow sawtooth per chapter (L1 comfort → L2 teach → L3 combine → L4 peak → L5 easy emotional decompression); Zeigarnik (keepsake shelf 12 slots with dashed silhouettes of missing items, 2 pre-seeded = endowed progress); 3 cosmetic "packing stamps"/level; honest DDA (hint-shimmer timing 45 s→15 s + gentle level variants only — never mid-level mutation, never rigged); daily "Memory of the Day" (30 pre-baked remixes) with pause-not-break streaks; no notifications by default; variable reward on cosmetic keepsakes only (1-in-3 replay, pity at 3rd).
- **Business model:** free, no monetization at launch (user decision).
- **Art:** flat-shaded low-poly, single shared Unlit material + one 256×256 gradient-palette texture (16 bands × 16 px), day + dusk variants. 12-color "Grandmother's House" palette. No outlines, no real-time shadows (blob decal under dragged piece), no post-processing. Budgets: packables 40–120 tris, furniture 100–300, heroes ≤500, room shells 800–1500.
- **Assets:** ~73 unique packables — ~48 re-paletted CC0 (Kenney Furniture/Food kits primary, Quaternius secondary) + ~25 self-made in Blender. UI: Kenney UI pack 9-slices tinted + Nunito (OFL) subset. Build Kitchen 100% first to validate pipeline.
- **Audio:** 4 lo-fi felt-piano loops in LMMS, all arrangements of ONE 4-bar motif. Fallback: FreePD CC0 piano. 20 SFX. Phone-speaker rules: high-pass 150 Hz, mono-sum check, place-sound ×3 round-robin ±4 cents. Audio budget ≈ 4.6 MB.
- **APK ledger:** UE base ≈ 80 MB + assets ≈ 10 MB → ≤ 100 MB green.

## Project location & architecture

Create at `/home/anshukushwaha/95095/Backup/Desktop/learn/homeward/` (new git repo).

```text
Homeward/
├── Config/               # DefaultEngine.ini, DefaultGame.ini
├── Content/              # All UASSETs
│   ├── Art/              # Meshes, Textures, Materials
│   ├── Audio/            # SoundCues, Wavs
│   ├── Data/             # DataAssets (Levels, Pieces, Chapters)
│   ├── Blueprints/       # BP subclasses, UI Widgets (UMG)
│   ├── Maps/             # Levels (Main, Vignettes)
│   └── UI/               # Slate/UMG textures
├── Source/
│   ├── Homeward/
│   │   ├── Homeward.Build.cs
│   │   ├── Homeward.cpp
│   │   ├── Homeward.h
│   │   ├── Core/         # GameMode, GameState, GameInstance
│   │   └── Logic/        # GridModel, SaveManager, DragController
│   ├── Homeward.Target.cs
│   └── HomewardEditor.Target.cs
└── Homeward.uproject
```

- **Data:** level/piece/chapter definitions = `UDataAsset` subclasses in C++, instanced in Editor. `PieceDef`: id, `cells: TArray<FIntVector>`, mesh, palette_row, flags. `LevelDef`: grid_size, pieces, par_seconds, etc.
- **Saves:** `USaveGame` subclass, JSON/Binary serialization in `Saved/` dir. Atomic tmp-write → rename, debounced.
- **Patterns:** C++ for core grid logic (unit tested), Delegates/Events up, function calls down. `UGameInstance` for global state.
- **Special pieces (one wordless teaching level each):** Fragile = topmost layer only (Ch2), Heavy = bottom layer only (Ch3), Tall h=2 reserves column above (Ch4), Keepsake = pack-or-keep choice (Ch1 L4).
- **FTUE (first 60 s):** 3 micro-levels — 1 piece/1 slot → vertical piece vs horizontal slot (forces rotate) → 4 pieces/3 slots. No text; hand icon only on demonstrated need.

## Dev workflow (CPU-only build, GPU later)

- Development currently relies on headless compiling via UnrealAutomationTool (UAT) since the Linux machine lacks a capable GPU for the Editor.
- Logic is written in C++ and can be compiled and unit tested headlessly.
- Real test target = physical Android phone: Package APK headlessly → `adb install` → test.
- Toolchain: Unreal Engine 5.8 (Linux), Clang, Android SDK/NDK, JDK 17.
- Size pass at M3: Strip unused plugins, pak compression, disabled editor content in shipping build.
- Art tools: Blender, Krita/Inkscape, LMMS + ChipTone. All CPU-fine.

## Milestones (1 session ≈ 2–3 h; total ≈ 55–75 sessions ≈ 6–8 months at 2–3/week)

- **M0 — Walking skeleton (3–5 sessions):** UE 5.8 install, git init, headless package script, save-stub round-trip, basic C++ structure compiled. *Accept:* Headless build completes < 5 min; APK installs.
- **M1 — Core loop greybox (10–15 sessions):** GridModel + headless tests; drag/snap/rotate/ghost; undo + shake-reset; win detection; 6 LevelDefs incl. FTUE trio.
- **M2 — Kitchen, real art & sound (10–14 sessions):** palette pipeline proven; 14 Kitchen packables + room shell; lighting; stamps; room-complete celebration; vignette player. *Accept:* Chapter 1 end-to-end at shipped quality.
- **M3 — Vertical slice (12–16 sessions):** Ch2 Living Room; full SaveGame; keepsake shelf; house map; settings; size optimization. *Accept:* APK ≤ 100 MB measured.
- **M4 — Full v1 (20–28 sessions):** Ch3–6, 4 more vignettes, tracks 2–4, DDA + variants, tuning, low-end perf pass, closed test track.

## Verification

- Per milestone: acceptance criteria above, checked on physical phone via APK sideload.
- GridModel logic: C++ unit tests run via UnrealAutomationTool.
- Level validity: offline DLX solver script verifies every LevelDef is exact-fit solvable before commit.

## Defaults taken (flag if wrong)

1. Scope = 6 chapters/28 levels (8-room version cut; +2–3 months if restored).
2. Music = self-composed LMMS motif suite (~30–40 h).
3. Project dir = `~/95095/Backup/Desktop/learn/homeward/`.

## First implementation step (M0, session 1) -> COMPLETED

~~Create basic Unreal Engine project structure (`.uproject`, `Source/`, `Target.cs`), generate Build files, and prepare headless build script.~~

**Status update (latest session):**
- `FGridModel` now enforces the actual mechanics: active-layer packing order, Fragile (top layer only), Heavy (bottom layer only), Tall (reserves the cell above) -- previously it had zero placement rules.
- `SettingsManager`, `DynamicDifficultyManager`, `MemoryOfTheDayManager` filled in (were no-op stubs; the daily-streak logic was dead code, never called, never persisted -- now lives in `UHomewardSaveGame` and is wired through `USaveManager`).
- New `APiece` (in-world item) and `APackingBox` (owns `FGridModel`, grid<->world coordinate conversion) classes; `UDragController` actually does raycast/drag/tap-rotate/place now (was 100% comment stubs). Removed a duplicate, disconnected touch-input state that used to live on `AHomewardPlayerController` alongside `UDragController`'s own.
- `AHomewardGameMode` now owns the level-flow glue: `StartLevel`/`OnLevelCompleted` connect `APackingBox::IsLevelComplete()` to DDA's gentle-variant check, chapter/level advancement (via `OnChapterCompleted`/`OnLevelAdvanceRequested` delegates), and Memory-of-the-Day completion recording. `AVignettePlayer::PlayVignette` now takes a `UChapterDef*` and can bind directly to `OnChapterCompleted`.
- `scripts/generate_greybox_meshes.py`: 6 -> 28 meshes (all polyomino shapes + hero/story props + Tall props).
- `scripts/dlx_solver.py`: fixed to model Fragile/Heavy/Tall exactly like `FGridModel::CanPlacePiece` (it used to validate a looser "tileable in any layer order" problem, not "placeable one layer at a time").
- `scripts/level_data.py`: all 6 chapters / 28 levels authored, matching the solver's schema, with hand-verified tilings documented inline. **Not yet run through the solver** -- do that before trusting any of it, especially Chapter 5 (Attic), which combines Fragile+Heavy+Tall in single levels.
- Test coverage added: `GridModelTests`, `PackingBoxTests`, `PieceTests` (all `Homeward.Logic.*` automation tests). `DragController` has none -- it needs a live World/PlayerController/camera to raycast against, so it's exercised on-device/in-PIE instead, per this doc's own Verification section.
- **None of the above has been compiled.** See `AGENTS.md`/session notes for how to compile headlessly via the Docker UE image (`build_docker.sh`'s image, but running just `Build.sh HomewardEditor` first is faster than a full `BuildCookRun`).

## Next implementation step (GPU Phase)

**Action Required:** Open `Homeward.uproject` in the Unreal Engine 5.8 Editor. Bind the C++ Subsystems and DataAssets to visual Blueprints, materials, and static meshes to begin grayboxing the Kitchen level (M2) -- use `scripts/level_data.py` for the actual `LevelDef`/`PieceDef` content and `Content/Art/Greybox/*.obj` for placeholder meshes. Also still open regardless of GPU: run `dlx_solver.py` against `level_data.py`, and compile the module (see above) to catch anything wrong in the C++ from this session.
