#!/usr/bin/env python3
import os

def write_obj(filename, vertices, faces):
    with open(filename, 'w') as f:
        f.write("# Homeward Procedural Greybox Mesh\n")
        for v in vertices:
            f.write(f"v {v[0]:.4f} {v[1]:.4f} {v[2]:.4f}\n")
        for face in faces:
            # OBJ indices are 1-based
            f.write("f")
            for idx in face:
                f.write(f" {idx+1}")
            f.write("\n")
    print(f"Generated {os.path.basename(filename)}")

def generate_cube(filename, size_x, size_y, size_z):
    x, y, z = size_x, size_y, size_z
    vertices = [
        (0, 0, 0), (x, 0, 0), (x, y, 0), (0, y, 0), # Bottom (0-3)
        (0, 0, z), (x, 0, z), (x, y, z), (0, y, z)  # Top (4-7)
    ]
    # Faces: CCW
    faces = [
        (0, 1, 2, 3), # Bottom
        (4, 5, 6, 7), # Top
        (0, 1, 5, 4), # Front
        (1, 2, 6, 5), # Right
        (2, 3, 7, 6), # Back
        (3, 0, 4, 7)  # Left
    ]
    write_obj(filename, vertices, faces)

def generate_l_shape(filename):
    # 2x2 L-Shape (missing one 1x1 corner)
    vertices = [
        (0,0,0), (2,0,0), (2,1,0), (1,1,0), (1,2,0), (0,2,0), # Bottom (0-5)
        (0,0,1), (2,0,1), (2,1,1), (1,1,1), (1,2,1), (0,2,1)  # Top (6-11)
    ]
    faces = [
        (0, 1, 2, 3, 4, 5), # Bottom
        (6, 7, 8, 9, 10, 11), # Top
        (0, 1, 7, 6), # Front
        (1, 2, 8, 7), # Right
        (2, 3, 9, 8), # Inner front
        (3, 4, 10, 9), # Inner right
        (4, 5, 11, 10), # Back
        (5, 0, 6, 11)  # Left
    ]
    write_obj(filename, vertices, faces)

def generate_polycube(filename, cells, cell_size=1.0):
    """Greybox mesh as a union of unit cubes at the given integer (x, y, z)
    cell offsets. Internal faces between adjacent cubes are left in place
    (unmerged) -- fine for a greybox proxy, and it makes this function
    trivially correct for any polyomino/polycube shape instead of having to
    hand-author a dedicated vertex/face list per piece the way
    generate_l_shape() does."""
    vertices = []
    faces = []
    s = cell_size
    for (cx, cy, cz) in cells:
        base = len(vertices)
        ox, oy, oz = cx * s, cy * s, cz * s
        vertices.extend([
            (ox, oy, oz), (ox + s, oy, oz), (ox + s, oy + s, oz), (ox, oy + s, oz),
            (ox, oy, oz + s), (ox + s, oy, oz + s), (ox + s, oy + s, oz + s), (ox, oy + s, oz + s),
        ])
        faces.extend([
            (base + 0, base + 1, base + 2, base + 3),  # bottom
            (base + 4, base + 5, base + 6, base + 7),  # top
            (base + 0, base + 1, base + 5, base + 4),  # front
            (base + 1, base + 2, base + 6, base + 5),  # right
            (base + 2, base + 3, base + 7, base + 6),  # back
            (base + 3, base + 0, base + 4, base + 7),  # left
        ])
    write_obj(filename, vertices, faces)

# Polyomino footprints named per the mechanics spec (Section 2, Piece
# taxonomy). Cells are the exact PieceDef.Cells a level author would use
# before any yaw rotation. A single shape is reused across rooms under
# different palette rows / hero-prop dressing, so there is one greybox
# mesh per *shape*, not per skin.
POLYOMINOES = {
    "Piece_1x1x1":      [(0, 0, 0)],
    "Piece_2x1x1":      [(0, 0, 0), (1, 0, 0)],
    "Piece_3x1x1":      [(0, 0, 0), (1, 0, 0), (2, 0, 0)],
    "Piece_L_Tromino":  [(0, 0, 0), (1, 0, 0), (0, 1, 0)],
    "Piece_O_Tetromino": [(0, 0, 0), (1, 0, 0), (0, 1, 0), (1, 1, 0)],
    "Piece_I_Tetromino": [(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0)],
    "Piece_L_Tetromino": [(0, 0, 0), (0, 1, 0), (0, 2, 0), (1, 0, 0)],
    "Piece_S_Tetromino": [(1, 0, 0), (2, 0, 0), (0, 1, 0), (1, 1, 0)],
    "Piece_T_Tetromino": [(0, 0, 0), (1, 0, 0), (2, 0, 0), (1, 1, 0)],
    "Piece_P_Pentomino": [(0, 0, 0), (1, 0, 0), (0, 1, 0), (1, 1, 0), (0, 2, 0)],
    "Piece_Big_2x3":     [(0, 0, 0), (1, 0, 0), (2, 0, 0), (0, 1, 0), (1, 1, 0), (2, 1, 0)],
    # Legacy shape name kept alongside Piece_L_Shape for the original
    # hand-authored 2x2-minus-corner variant (a slightly different L than
    # Piece_L_Tromino above) -- see generate_l_shape().
}

# Hero/story props (Section 4 art bible): the ~25 self-made items no CC0
# pack can supply because they carry the story (teapot, music box, etc).
# These are 1x1x1-footprint pieces visually -- Cells stays a single cell in
# the LevelDef; only the greybox proxy's shape/height varies for flavor.
# Sized as (size_x, size_y, size_z) boxes; an artist reshapes these later,
# the proxy just needs to read at-a-glance in a greybox pass.
HERO_PROPS = {
    "Prop_Teapot_Proxy":            (0.8, 0.8, 0.6),
    "Prop_MusicBox_Proxy":          (0.5, 0.5, 0.5),
    "Prop_RotaryPhone_Proxy":       (0.7, 0.6, 0.5),
    "Prop_SewingMachine_Proxy":     (0.9, 0.6, 0.7),
    "Prop_Teddy_Proxy":             (0.6, 0.5, 0.8),
    "Prop_RecordPlayer_Proxy":      (0.9, 0.9, 0.4),
    "Prop_Radio_Proxy":             (0.7, 0.5, 0.6),
    "Prop_WeddingAlbum_Proxy":      (0.7, 0.9, 0.15),
    "Prop_AlarmClock_Proxy":        (0.4, 0.4, 0.4),
    "Prop_PictureFrame_Proxy":      (0.7, 0.9, 0.1),
    "Prop_Candlesticks_Proxy":      (0.3, 0.3, 0.7),
    "Prop_ToolBox_Proxy":           (0.9, 0.5, 0.5),
    "Prop_IronPot_Proxy":           (0.6, 0.6, 0.5),
}

# Tall (h=2) hero props: the piece's own footprint is 1 cell on the active
# layer, but the object itself is visually tall -- the reserved cell above
# is a translucent-column *material* concern (Blueprint/editor glue), not a
# mesh-generation one, so these greybox proxies are just taller boxes.
TALL_HERO_PROPS = {
    "Prop_Vase_Proxy":              (0.5, 0.5, 1.8),
    "Prop_StandingMirror_Proxy":    (0.6, 0.2, 1.9),
    "Prop_GrandfatherClockPiece_Proxy": (0.6, 0.6, 1.9),
}

if __name__ == "__main__":
    out_dir = os.path.join(os.path.dirname(__file__), "..", "Content", "Art", "Greybox")
    os.makedirs(out_dir, exist_ok=True)

    print("Generating M2 Greybox Meshes...")

    for name, cells in POLYOMINOES.items():
        generate_polycube(os.path.join(out_dir, f"{name}.obj"), cells)

    # Original hand-authored L shape kept for backwards compatibility with
    # anything already referencing Piece_L_Shape.obj.
    generate_l_shape(os.path.join(out_dir, "Piece_L_Shape.obj"))

    for name, (sx, sy, sz) in HERO_PROPS.items():
        generate_cube(os.path.join(out_dir, f"{name}.obj"), sx, sy, sz)

    for name, (sx, sy, sz) in TALL_HERO_PROPS.items():
        generate_cube(os.path.join(out_dir, f"{name}.obj"), sx, sy, sz)

    print(f"Done. {len(POLYOMINOES) + 1 + len(HERO_PROPS) + len(TALL_HERO_PROPS)} greybox meshes.")
