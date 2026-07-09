#!/usr/bin/env python3
"""
Homeward level content, in the same piece-dict schema dlx_solver.py expects
(see generate_matrix_for_level's docstring): each piece is
{"cells": [(x,y,z), ...], "is_heavy": bool, "is_fragile": bool, "is_tall": bool}.
grid_size is (Width, Depth, Layers) -- matches FIntVector GridSize on
ULevelDef exactly (see LevelDef.h's "Width, Depth, Layers (Height)" comment
and GridModel.cpp, where GridSize.Z is the layer axis).

STATUS: all 6 chapters / 28 levels below are filled in. Every level was
tiled and checked by hand, layer by layer, cell by cell -- shown in the
comment above each level -- because this was authored without being able
to run dlx_solver.py to confirm it. That hand-verification gets harder to
trust as the grid grows and specials combine (Chapter 5's Attic, which
mixes Fragile+Heavy+Tall in one level, is the riskiest content here).

RUN dlx_solver.py's generate_matrix_for_level() + search() against every
level in CHAPTERS before treating any of this as ship-ready -- that is
exactly what the solver exists for, and this module has never actually
been executed against it.

Before trusting ANY level here (including Chapter 1) as ship-ready, run it
through dlx_solver.py's generate_matrix_for_level() + search() and confirm
a solution is found -- that's exactly what the solver exists for, this
module has just never been executed against it.
"""

# Per-chapter grid size. GridSize.Z (Layers) is the binding constraint for
# each chapter's headline mechanic, per PLAN.md's "Special pieces" list:
#   Ch2 Fragile needs >=2 layers (it's only legal on the topmost one)
#   Ch3 Heavy needs >=2 layers (only legal on the bottom one)
#   Ch4 Tall needs a layer above the active one to reserve into
# Footprint (Width, Depth) grows alongside layers so later chapters are
# also just bigger, not only mechanically denser.
CHAPTERS = {
    "Ch1_Kitchen":     {"grid_size": (3, 3, 1), "levels": []},  # filled below
    "Ch2_LivingRoom":  {"grid_size": (4, 3, 2), "levels": []},  # filled below
    "Ch3_Bedroom":     {"grid_size": (4, 4, 2), "levels": []},  # filled below
    "Ch4_ChildsRoom":  {"grid_size": (4, 4, 3), "levels": []},  # filled below
    "Ch5_Attic":       {"grid_size": (5, 4, 3), "levels": []},  # filled below
    "Ch6_Epilogue":    {"grid_size": (4, 3, 2), "levels": []},  # filled below (3 levels, reversal/coda)
}

def piece(cells, is_heavy=False, is_fragile=False, is_tall=False, is_keepsake=False):
    return {
        "cells": cells,
        "is_heavy": is_heavy,
        "is_fragile": is_fragile,
        "is_tall": is_tall,
        "is_keepsake": is_keepsake,  # not read by the solver; carried through for LevelDef authoring
    }

# ---------------------------------------------------------------------------
# Chapter 1: Kitchen -- grid (3, 3, 1), 9 cells, single layer, no special
# pieces (Fragile/Heavy/Tall are introduced in later chapters). Sawtooth:
# L1 comfort -> L2 teach rotate -> L3 combine -> L4 peak + keepsake choice
# -> L5 easy decompression.
# ---------------------------------------------------------------------------

ch1_levels = []

# L1 -- comfort re-entry. Three straight 3-cell pieces, each filling one
# full row. Tiling: row y=0, y=1, y=2 each taken whole -- trivially exact
# (3 disjoint rows x 3 cells = 9 = grid volume), no rotation required.
ch1_levels.append({
    "level_id": "Ch1_L1",
    "par_seconds": 45,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),
    ],
})

# L2 -- teach rotate. Verified tiling:
#   DominoA horizontal @ (0,0)-(1,0)
#   DominoB VERTICAL (rotated 90 from its authored horizontal default) @ (2,0)-(2,1)
#   DominoC horizontal @ (0,1)-(1,1)
#   Tromino horizontal @ (0,2)-(1,2)-(2,2)
# Covers all 9 cells exactly once; DominoB only fits its column slot after
# a 90-degree tap-rotate, which is the whole point of this level.
ch1_levels.append({
    "level_id": "Ch1_L2",
    "par_seconds": 60,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0), (1, 0, 0)]),  # DominoA, ends up at (0,0)-(1,0)
        piece([(0, 0, 0), (1, 0, 0)]),  # DominoB, ends up rotated to (2,0)-(2,1)
        piece([(0, 0, 0), (1, 0, 0)]),  # DominoC, ends up at (0,1)-(1,1)
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),  # Tromino, (0,2)-(2,2)
    ],
})

# L3 -- combine, moderate density. Verified tiling:
#   O-tetromino (2x2 block) @ (0,0)-(1,1)
#   Domino VERTICAL @ (2,0)-(2,1)
#   Tromino horizontal @ (0,2)-(2,2)
ch1_levels.append({
    "level_id": "Ch1_L3",
    "par_seconds": 75,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0), (1, 0, 0), (0, 1, 0), (1, 1, 0)]),  # O-tetromino
        piece([(0, 0, 0), (1, 0, 0)]),                         # domino, rotated vertical
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),              # tromino
    ],
})

# L4 -- peak difficulty + first Keepsake (pack-or-keep choice), per
# PLAN.md's "Keepsake = pack-or-keep choice (Ch1 L4)". Verified tiling
# (a checkerboard-parity domino tiling of a 3x3 board with one corner
# removed -- see level_data.py's authoring notes for the parity argument):
#   Keepsake single @ (0,0)
#   Domino horizontal @ (1,0)-(2,0)
#   Domino VERTICAL @ (0,1)-(0,2)
#   Domino VERTICAL @ (1,1)-(1,2)
#   Domino VERTICAL @ (2,1)-(2,2)
# 5 pieces (highest count in the chapter) and 3 of the 4 dominoes need a
# 90-degree rotation to fit -- genuinely harder than L1-L3 on piece count
# and rotation load, the two difficulty levers the mechanics spec ranks
# above raw footprint gnarliness for a grid this small.
ch1_levels.append({
    "level_id": "Ch1_L4",
    "par_seconds": 110,
    "keepsake_id": "Keepsake_Ch1_Teapot",
    "pieces": [
        piece([(0, 0, 0)], is_keepsake=True),
        piece([(0, 0, 0), (1, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0)]),
    ],
})

# L5 -- easy decompression, leads into the chapter vignette. Verified
# tiling: Big 2x3 block @ rows y=0-1 (all of x=0,1,2), then a horizontal
# domino + single filling the remaining row y=2.
ch1_levels.append({
    "level_id": "Ch1_L5",
    "par_seconds": 40,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (0, 1, 0), (1, 1, 0), (2, 1, 0)]),  # Big 2x3
        piece([(0, 0, 0), (1, 0, 0)]),  # domino
        piece([(0, 0, 0)]),             # single
    ],
})

CHAPTERS["Ch1_Kitchen"]["levels"] = ch1_levels

# ---------------------------------------------------------------------------
# Chapter 2: Living Room -- grid (4, 3, 2), 24 cells over 2 layers (12 each).
# Introduces Fragile (topmost layer only -- layer index 1 here). Sawtooth:
# L1 comfort (first time in a 2-layer box) -> L2 teach Fragile -> L3
# combine (Fragile again + more pieces) -> L4 peak (two Fragile pieces,
# most pieces in the chapter) -> L5 easy decompression, no Fragile.
#
# Every level below fills layer 0 and layer 1 as two INDEPENDENT 4x3 exact
# covers (each piece's authored cells are single-layer; only z differs
# between "layer0 pieces" and "layer1 pieces" in the lists below). That
# keeps hand-verification to the same 2D row/column reasoning used in
# Chapter 1, repeated once per layer, rather than reasoning about the two
# layers jointly.
# ---------------------------------------------------------------------------

ch2_levels = []

# L1 -- comfort. Both layers: 3 straight I-tetrominoes, one per row
# (y=0,1,2), each layer's own rows trivially disjoint and exact (3x4=12).
ch2_levels.append({
    "level_id": "Ch2_L1",
    "par_seconds": 60,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer0 row0
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer0 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer0 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer1 row0
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer1 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer1 row2
    ],
})

# L2 -- teach Fragile. Layer0: same 3-row comfort baseline as L1 (attention
# stays on the new mechanic, not on layer 0). Layer1 (12 cells) verified
# tiling: Fragile single @ (3,2) [top layer's far corner]; I-tetromino
# fills row y=0 (4); I-tetromino fills row y=1 (4); row y=2 minus the
# fragile corner leaves (0,2),(1,2),(2,2) -- exactly a straight tromino.
# 1+4+4+3 = 12, disjoint, exact.
ch2_levels.append({
    "level_id": "Ch2_L2",
    "par_seconds": 75,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer0 row0
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer0 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer0 row2
        piece([(0, 0, 0)], is_fragile=True),                   # layer1 fragile corner
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer1 row0
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer1 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),              # layer1 row2 minus corner
    ],
})

# L3 -- combine: more pieces on both layers than L2. Layer0 verified
# tiling: each of the 3 rows split into 2 horizontal dominoes (2+2=4 per
# row x 3 rows = 12), 6 pieces, no rotation needed but noticeably more
# pieces than L1/L2's layer0. Layer1: identical fragile-corner tiling to
# L2 (reinforcing the just-taught mechanic while layer0 carries the extra
# difficulty this time).
ch2_levels.append({
    "level_id": "Ch2_L3",
    "par_seconds": 90,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row0, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row1, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row2, 2 dominoes
        piece([(0, 0, 0)], is_fragile=True),                            # layer1 fragile corner
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),           # layer1 row0
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),           # layer1 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                       # layer1 row2 minus corner
    ],
})

# L4 -- chapter peak, most pieces of the 5. Layer0 verified tiling: rows
# y=0,1 each split into 2 horizontal dominoes (8 cells, 4 pieces); row y=2
# split into 4 singles (4 cells, 4 pieces) -- 8 pieces total, smaller
# average piece size than any earlier level this chapter. Layer1 verified
# tiling: TWO fragile singles at opposite corners (0,0) and (3,2); row y=0
# minus (0,0) leaves (1,0),(2,0),(3,0) = a straight tromino; row y=1 is
# untouched, a full I-tetromino; row y=2 minus (3,2) leaves (0,2),(1,2),(2,2)
# = a straight tromino. 1+1+3+4+3 = 12, disjoint, exact. 5 pieces on
# layer1 + 8 on layer0 = 13 pieces, the chapter's high-water mark.
ch2_levels.append({
    "level_id": "Ch2_L4",
    "par_seconds": 130,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row0, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row1, 2 dominoes
        piece([(0, 0, 0)]), piece([(0, 0, 0)]), piece([(0, 0, 0)]), piece([(0, 0, 0)]),  # layer0 row2, 4 singles
        piece([(0, 0, 0)], is_fragile=True),                            # layer1 fragile corner A
        piece([(0, 0, 0)], is_fragile=True),                            # layer1 fragile corner B
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                       # layer1 row0 minus corner A
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),           # layer1 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                       # layer1 row2 minus corner B
    ],
})

# L5 -- easy decompression, no Fragile this time (ease off before the
# vignette). Both layers: same 3-row comfort tiling as L1.
ch2_levels.append({
    "level_id": "Ch2_L5",
    "par_seconds": 55,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
    ],
})

CHAPTERS["Ch2_LivingRoom"]["levels"] = ch2_levels

# ---------------------------------------------------------------------------
# Chapter 3: Bedroom -- grid (4, 4, 2), 32 cells over 2 layers (16 each).
# Introduces Heavy (bottom layer only -- layer index 0). Same sawtooth
# shape as Chapter 2, mirrored: L1 comfort -> L2 teach Heavy -> L3 combine
# -> L4 peak (two Heavy pieces, most pieces) -> L5 easy, no Heavy.
#
# Every layer is verified the same way as Chapter 2: rows (4 cells each,
# 4 rows per 4x4 layer) partitioned into straight pieces/dominoes/singles,
# with a flagged single carved out of a row when a special piece needs a
# specific corner.
# ---------------------------------------------------------------------------

ch3_levels = []

# L1 -- comfort re-entry into the bigger 4x4 footprint. Both layers: 4
# straight I-tetrominoes, one per row (y=0..3), trivially exact (4x4=16).
ch3_levels.append({
    "level_id": "Ch3_L1",
    "par_seconds": 70,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer0 row0
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer0 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer0 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer0 row3
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer1 row0
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer1 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer1 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),  # layer1 row3
    ],
})

# L2 -- teach Heavy. Layer0 (16 cells) verified tiling: Heavy single @
# (0,0) [bottom layer's corner]; row0 minus that corner leaves (1,0),(2,0),
# (3,0) -- a straight tromino; rows 1-3 each a full I-tetromino.
# 1+3+4+4+4 = 16, disjoint, exact. Layer1: comfort baseline, same 4-row
# tiling as L1.
ch3_levels.append({
    "level_id": "Ch3_L2",
    "par_seconds": 85,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0)], is_heavy=True),                     # layer0 heavy corner
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),              # layer0 row0 minus corner
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),   # layer0 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),   # layer0 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),   # layer0 row3
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),   # layer1 row0
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),   # layer1 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),   # layer1 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),   # layer1 row3
    ],
})

# L3 -- combine: more pieces on both layers. Layer0 verified tiling: Heavy
# corner @ (0,0); row0 remainder = tromino (3); rows 1-2 each split into 2
# horizontal dominoes (2+2=4 per row); row3 a full I-tetromino (4).
# 1+3+4+4+4 = 16. Layer1 verified tiling: all 4 rows split into 2 dominoes
# each (4x4=16), 8 pieces -- more pieces than L1/L2's layer1.
ch3_levels.append({
    "level_id": "Ch3_L3",
    "par_seconds": 100,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0)], is_heavy=True),                     # layer0 heavy corner
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),              # layer0 row0 minus corner
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row1, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row2, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),   # layer0 row3
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row0, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row1, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row2, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row3, 2 dominoes
    ],
})

# L4 -- chapter peak, most pieces. Layer0 verified tiling: TWO Heavy
# singles at opposite corners (0,0) and (3,3); row0 minus (0,0) leaves a
# tromino (1,0)-(3,0); row3 minus (3,3) leaves a tromino (0,3)-(2,3); rows
# 1-2 each split into 2 dominoes. 1+1+3+3+2+2+2+2 = 16, disjoint, exact.
# Layer1 verified tiling: all 4 rows split into 2 dominoes each, same as
# L3's layer1 (16 cells, 8 pieces). Total 8+8 = 16 pieces, the chapter's
# high-water mark.
ch3_levels.append({
    "level_id": "Ch3_L4",
    "par_seconds": 140,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0)], is_heavy=True),                     # layer0 heavy corner A
        piece([(0, 0, 0)], is_heavy=True),                     # layer0 heavy corner B
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),              # layer0 row0 minus corner A
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),              # layer0 row3 minus corner B
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row1, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row2, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row0, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row1, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row2, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row3, 2 dominoes
    ],
})

# L5 -- easy decompression, no Heavy this time. Both layers: same 4-row
# comfort tiling as L1.
ch3_levels.append({
    "level_id": "Ch3_L5",
    "par_seconds": 65,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
    ],
})

CHAPTERS["Ch3_Bedroom"]["levels"] = ch3_levels

# ---------------------------------------------------------------------------
# Chapter 4: Child's Room -- grid (4, 4, 3), 48 cells over 3 layers (16
# each). Introduces Tall (own cell on the active layer + reserves the same
# cell one layer up). Every Tall piece below is anchored at layer 0's
# (0,0) corner (or, in L4, also (3,3)) -- so layer 1's "own-cell" pieces
# only need to cover 16 minus however many cells are reserved from below;
# layer 2 (top) is never a Tall target here (no layer above it to reserve
# into), so it's always a plain 16-cell exact cover. Sawtooth: L1 comfort
# -> L2 teach Tall -> L3 combine -> L4 peak (two Tall pieces, most pieces)
# -> L5 easy, no Tall.
# ---------------------------------------------------------------------------

ch4_levels = []

# L1 -- comfort re-entry into a 3-layer box. All 3 layers: 4 straight
# I-tetrominoes, one per row -- trivially exact (4x4=16 per layer).
ch4_levels.append({
    "level_id": "Ch4_L1",
    "par_seconds": 90,
    "keepsake_id": None,
    "pieces": [piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]) for _ in range(12)],
})

# L2 -- teach Tall. Verified tiling: Tall single @ layer0 (0,0) -- its own
# cell fills layer0's corner, its reservation fills layer1's (0,0) for
# free. Layer0 remainder (15 cells): row0 minus the corner = tromino (3);
# rows 1-3 each a full I-tetromino (4+4+4=12); 3+12=15, +1 (the Tall's own
# cell) = 16. Layer1 remainder (15 own-cell cells, since (0,0) there is
# already filled by the reservation): identical shape to layer0's
# remainder -- tromino (3) + 3 I-tetrominoes (12) = 15, +1 reserved = 16.
# Layer2 (top, untouched by the reservation): plain 4-row exact cover.
ch4_levels.append({
    "level_id": "Ch4_L2",
    "par_seconds": 110,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0)], is_tall=True),                       # layer0 tall corner (reserves layer1's same cell)
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                # layer0 row0 minus corner
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer0 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer0 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer0 row3
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                # layer1 row0 minus reserved corner
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer1 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer1 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer1 row3
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer2 row0
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer2 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer2 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer2 row3
    ],
})

# L3 -- combine: Tall again (reinforcing), more pieces on layer0/layer1
# via dominoes, and layer2 stepped up to an all-domino tiling too.
# Layer0 verified: tall(1) + row0 remainder tromino(3) + rows1-2 as 2
# dominoes each (8) + row3 I-tetromino(4) = 16. Layer1 verified: row0
# remainder tromino(3, reserved corner already filled) + rows1-2 as 2
# dominoes each (8) + row3 I-tetromino(4) = 15 own-cell + 1 reserved = 16.
# Layer2 verified: all 4 rows as 2 dominoes each = 16.
ch4_levels.append({
    "level_id": "Ch4_L3",
    "par_seconds": 130,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0)], is_tall=True),                        # layer0 tall corner
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                # layer0 row0 minus corner
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row1, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row2, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer0 row3
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                # layer1 row0 minus reserved corner
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row1, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row2, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),     # layer1 row3
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer2 row0, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer2 row1, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer2 row2, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer2 row3, 2 dominoes
    ],
})

# L4 -- chapter peak: TWO Tall pieces (layer0 corners (0,0) and (3,3)),
# most pieces of the 5. Layer0 verified: 2 tall(2) + row0 minus (0,0) =
# tromino(3) + row3 minus (3,3) = tromino(3) + rows1-2 as 2 dominoes each
# (8) = 16. Layer1 verified: row0 minus reserved (0,0) = tromino(3) +
# row3 minus reserved (3,3) = tromino(3) + rows1-2 as 2 dominoes each (8)
# = 14 own-cell + 2 reserved = 16. Layer2: same all-domino tiling as L3.
# Total pieces: layer0=8, layer1=6, layer2=8 -> 22, this chapter's peak.
ch4_levels.append({
    "level_id": "Ch4_L4",
    "par_seconds": 170,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0)], is_tall=True),                        # layer0 tall corner A
        piece([(0, 0, 0)], is_tall=True),                        # layer0 tall corner B
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                # layer0 row0 minus corner A
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                # layer0 row3 minus corner B
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row1, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer0 row2, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                # layer1 row0 minus reserved corner A
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                # layer1 row3 minus reserved corner B
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row1, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer1 row2, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer2 row0, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer2 row1, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer2 row2, 2 dominoes
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]),  # layer2 row3, 2 dominoes
    ],
})

# L5 -- easy decompression, no Tall. All 3 layers: same 4-row comfort
# tiling as L1.
ch4_levels.append({
    "level_id": "Ch4_L5",
    "par_seconds": 80,
    "keepsake_id": None,
    "pieces": [piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]) for _ in range(12)],
})

CHAPTERS["Ch4_ChildsRoom"]["levels"] = ch4_levels

# ---------------------------------------------------------------------------
# Chapter 5: Attic -- grid (5, 4, 3), 60 cells over 3 layers (20 each, 4
# rows of 5). All-combined chapter: Fragile, Heavy, and Tall all appear.
# Heavy anchors layer0's (0,0) corner, Tall anchors layer0's (4,3) corner
# (reserving layer1's (4,3)), Fragile anchors layer2's (0,0) corner. A
# 5-wide row has no single straight piece in the shape library, so every
# full row is filled as I-tetromino(4) + single(1) (comfort levels) or
# domino+domino+single (denser levels); a row with a flagged corner
# removed loses exactly that filler pattern for a smaller one (tromino or
# I-tetromino), verified per level below.
# ---------------------------------------------------------------------------

ch5_levels = []

# L1 -- comfort re-entry into the widest grid yet. All 3 layers, no
# specials: each of the 4 rows per layer = I-tetromino(4) + single(1),
# 5 cells/row x 4 rows = 20/layer, exact.
ch5_levels.append({
    "level_id": "Ch5_L1",
    "par_seconds": 100,
    "keepsake_id": None,
    "pieces": (
        [piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]) for _ in range(12)] +
        [piece([(0, 0, 0)]) for _ in range(12)]
    ),
})

# L2 -- teach all three specials at once, each isolated in its own corner
# so their effects don't interact. Layer0 verified: Heavy @ (0,0), row0
# minus that corner = I-tetromino (1,0)-(4,0); Tall @ (4,3) (reserves
# layer1's (4,3)), row3 minus that corner = I-tetromino (0,3)-(3,3);
# rows 1-2 untouched, each I-tetromino(4)+single(1). 1+4+1+4+5+5 = 20.
# Layer1 verified: row3 minus the reserved (4,3) = I-tetromino (0,3)-(3,3)
# (4); rows 0-2 untouched (5 each) = 15; 4+15 = 19 own-cell + 1 reserved
# = 20. Layer2 verified: Fragile @ (0,0), row0 minus that corner =
# I-tetromino (1,0)-(4,0) (4); rows 1-3 untouched (5 each) = 15; 1+4+15=20.
ch5_levels.append({
    "level_id": "Ch5_L2",
    "par_seconds": 130,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0)], is_heavy=True),                       # layer0 heavy corner (0,0)
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),      # layer0 row0 minus heavy corner
        piece([(0, 0, 0)], is_tall=True),                         # layer0 tall corner (4,3)
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),      # layer0 row3 minus tall corner
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]), piece([(0, 0, 0)]),  # layer0 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]), piece([(0, 0, 0)]),  # layer0 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),      # layer1 row3 minus reserved corner
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]), piece([(0, 0, 0)]),  # layer1 row0
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]), piece([(0, 0, 0)]),  # layer1 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]), piece([(0, 0, 0)]),  # layer1 row2
        piece([(0, 0, 0)], is_fragile=True),                     # layer2 fragile corner (0,0)
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),      # layer2 row0 minus fragile corner
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]), piece([(0, 0, 0)]),  # layer2 row1
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]), piece([(0, 0, 0)]),  # layer2 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]), piece([(0, 0, 0)]),  # layer2 row3
    ],
})

# L3 -- combine: same three specials, same corners, but every untouched
# row now fills as domino+domino+single (3 pieces/row, 5 cells) instead of
# I-tetromino+single (2 pieces/row) -- more pieces per layer than L2 while
# every row/corner argument from L2 still applies unchanged.
ch5_levels.append({
    "level_id": "Ch5_L3",
    "par_seconds": 160,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0)], is_heavy=True),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0)], is_tall=True),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer0 row1
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer0 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),      # layer1 row3 minus reserved corner
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer1 row0
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer1 row1
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer1 row2
        piece([(0, 0, 0)], is_fragile=True),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer2 row1
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer2 row2
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer2 row3
    ],
})

# L4 -- chapter peak. Adds a SECOND Heavy corner rather than doubling
# every special (keeps the corner bookkeeping tractable by hand). Layer0
# verified: Heavy @ (0,0) and Heavy @ (4,0) -- both ends of row0 -- leaves
# row0's middle (1,0),(2,0),(3,0) as a tromino (3); Tall @ (4,3) as
# before, row3 minus it = I-tetromino (4); rows 1-2 as domino+domino+
# single (5 each). 1+1+3+1+4+5+5 = 20. Layer1: row3 minus the Tall
# reservation = I-tetromino (4); rows 0-2 as domino+domino+single each
# (15). 4+15 = 19 own-cell + 1 reserved = 20. Layer2: unchanged from L3
# (Fragile @ (0,0), row0 minus it = I-tetromino, rows 1-3 domino+domino+
# single). This level has the most pieces of the chapter.
ch5_levels.append({
    "level_id": "Ch5_L4",
    "par_seconds": 200,
    "keepsake_id": None,
    "pieces": [
        piece([(0, 0, 0)], is_heavy=True),                       # layer0 heavy corner A (0,0)
        piece([(0, 0, 0)], is_heavy=True),                       # layer0 heavy corner B (4,0)
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0)]),                 # layer0 row0 middle (both corners removed)
        piece([(0, 0, 0)], is_tall=True),                         # layer0 tall corner (4,3)
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),      # layer0 row3 minus tall corner
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer0 row1
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer0 row2
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),      # layer1 row3 minus reserved corner
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer1 row0
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer1 row1
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer1 row2
        piece([(0, 0, 0)], is_fragile=True),
        piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]),
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer2 row1
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer2 row2
        piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0), (1, 0, 0)]), piece([(0, 0, 0)]),  # layer2 row3
    ],
})

# L5 -- easy decompression, no specials. Same I-tetromino+single comfort
# tiling as L1, all 3 layers.
ch5_levels.append({
    "level_id": "Ch5_L5",
    "par_seconds": 95,
    "keepsake_id": None,
    "pieces": (
        [piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]) for _ in range(12)] +
        [piece([(0, 0, 0)]) for _ in range(12)]
    ),
})

CHAPTERS["Ch5_Attic"]["levels"] = ch5_levels

# ---------------------------------------------------------------------------
# Chapter 6: Epilogue -- grid (4, 3, 2), 24 cells over 2 layers (12 each,
# 3 rows of 4). "Epilogue reverses the verb: unpack into the new
# apartment" (PLAN.md) -- gameplay-wise the player drags pieces OUT of the
# box onto shelf slots instead of into a grid, but the underlying
# exact-cover problem (which cells are occupied vs empty) is identical
# regardless of pack/unpack direction, so the same piece-list schema and
# hand-verification method apply unchanged. No new specials here -- this
# is a short (3-level), gentle coda, not another full 5-level sawtooth.
# ---------------------------------------------------------------------------

ch6_levels = []

# L1 -- gentle open. Both layers: 3 straight I-tetrominoes, one per row
# (3 rows x 4 cells = 12/layer), no rotation needed.
ch6_levels.append({
    "level_id": "Ch6_L1",
    "par_seconds": 55,
    "keepsake_id": None,
    "pieces": [piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]) for _ in range(6)],
})

# L2 -- a touch more texture. Layer0: same 3-row comfort tiling as L1.
# Layer1 verified: each of the 3 rows split into 2 horizontal dominoes
# (2+2=4 per row x 3 rows = 12), 6 pieces on this layer alone.
ch6_levels.append({
    "level_id": "Ch6_L2",
    "par_seconds": 60,
    "keepsake_id": None,
    "pieces": (
        [piece([(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)]) for _ in range(3)] +
        [piece([(0, 0, 0), (1, 0, 0)]) for _ in range(6)]
    ),
})

# L3 -- closing level (reunion beat). Both layers split into dominoes (3
# rows x 2 dominoes each = 6 pieces/layer, 12 pieces total) -- slightly
# more pieces than L1/L2, but still no shape gnarliness: this is a
# gentle finale, not a difficulty peak the way each earlier chapter's L4
# was.
ch6_levels.append({
    "level_id": "Ch6_L3",
    "par_seconds": 70,
    "keepsake_id": None,
    "pieces": [piece([(0, 0, 0), (1, 0, 0)]) for _ in range(12)],
})

CHAPTERS["Ch6_Epilogue"]["levels"] = ch6_levels

if __name__ == "__main__":
    for chapter_id, chapter in CHAPTERS.items():
        n = len(chapter["levels"])
        print(f"{chapter_id}: grid={chapter['grid_size']}  levels_authored={n}")
