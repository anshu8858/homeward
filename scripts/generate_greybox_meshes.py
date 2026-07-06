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

if __name__ == "__main__":
    out_dir = os.path.join(os.path.dirname(__file__), "..", "Content", "Art", "Greybox")
    os.makedirs(out_dir, exist_ok=True)
    
    print("Generating M2 Greybox Meshes...")
    generate_cube(os.path.join(out_dir, "Piece_1x1x1.obj"), 1, 1, 1)
    generate_cube(os.path.join(out_dir, "Piece_2x1x1.obj"), 2, 1, 1)
    generate_cube(os.path.join(out_dir, "Piece_3x1x1.obj"), 3, 1, 1)
    generate_l_shape(os.path.join(out_dir, "Piece_L_Shape.obj"))
    generate_cube(os.path.join(out_dir, "Prop_Teapot_Proxy.obj"), 0.8, 0.8, 0.6)
    generate_cube(os.path.join(out_dir, "Prop_MusicBox_Proxy.obj"), 0.5, 0.5, 0.5)
    print("Done.")
