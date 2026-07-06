#!/usr/bin/env python3
import sys

# Dancing Links (DLX) Implementation for Homeward offline exact-fit validation
# Implements Algorithm X to quickly verify if a level configuration is exact-fit solvable.

class Node:
    def __init__(self, column=None):
        self.left = self
        self.right = self
        self.up = self
        self.down = self
        self.column = column
        self.row_id = None

class ColumnNode(Node):
    def __init__(self, name):
        super().__init__()
        self.size = 0
        self.name = name
        self.column = self

def create_dlx_matrix(headers, matrix_rows):
    header_node = ColumnNode("root")
    columns = []
    for h in headers:
        col = ColumnNode(h)
        col.left = header_node.left
        col.right = header_node
        header_node.left.right = col
        header_node.left = col
        columns.append(col)
        
    for row_id, cols_in_row in matrix_rows:
        prev_node = None
        for col_idx in cols_in_row:
            col = columns[col_idx]
            node = Node(col)
            node.row_id = row_id
            
            node.up = col.up
            node.down = col
            col.up.down = node
            col.up = node
            col.size += 1
            
            if prev_node:
                node.left = prev_node
                node.right = prev_node.right
                prev_node.right.left = node
                prev_node.right = node
            else:
                prev_node = node
    return header_node

def cover(col):
    col.right.left = col.left
    col.left.right = col.right
    i = col.down
    while i != col:
        j = i.right
        while j != i:
            j.down.up = j.up
            j.up.down = j.down
            j.column.size -= 1
            j = j.right
        i = i.down

def uncover(col):
    i = col.up
    while i != col:
        j = i.left
        while j != i:
            j.column.size += 1
            j.down.up = j
            j.up.down = j
            j = j.left
        i = i.up
    col.right.left = col
    col.left.right = col

def search(header, k, solution, results):
    if header.right == header:
        results.append(list(solution))
        return
    col = header.right
    c = header.right
    while c != header:
        if c.size < col.size:
            col = c
        c = c.right
    cover(col)
    r = col.down
    while r != col:
        solution.append(r.row_id)
        j = r.right
        while j != r:
            cover(j.column)
            j = j.right
        search(header, k + 1, solution, results)
        if results: return # Return on first solution found (we only need to know IF it's solvable)
        r = solution.pop()
        col = r.column
        j = r.left
        while j != r:
            uncover(j.column)
            j = j.left
        r = r.down
    uncover(col)

# --- Homeward specific setup ---

def rotate_cells_yaw(cells, rotations):
    rotated = []
    for x, y, z in cells:
        rx, ry = x, y
        for _ in range(rotations):
            rx, ry = -ry, rx
        rotated.append((rx, ry, z))
    return rotated

def generate_matrix_for_level(grid_size, pieces):
    W, H, D = grid_size
    headers = [f"Cell_{x}_{y}_{z}" for z in range(D) for y in range(H) for x in range(W)]
    headers += [f"Piece_{i}" for i in range(len(pieces))]
    
    matrix_rows = []
    for piece_idx, piece_cells in enumerate(pieces):
        for rot in range(4):
            rotated = rotate_cells_yaw(piece_cells, rot)
            min_x = min(c[0] for c in rotated)
            max_x = max(c[0] for c in rotated)
            min_y = min(c[1] for c in rotated)
            max_y = max(c[1] for c in rotated)
            min_z = min(c[2] for c in rotated)
            max_z = max(c[2] for c in rotated)
            
            for tz in range(-min_z, D - max_z):
                for ty in range(-min_y, H - max_y):
                    for tx in range(-min_x, W - max_x):
                        cols = []
                        valid = True
                        for cx, cy, cz in rotated:
                            nx, ny, nz = cx + tx, cy + ty, cz + tz
                            if 0 <= nx < W and 0 <= ny < H and 0 <= nz < D:
                                cell_idx = nz * (W * H) + ny * W + nx
                                cols.append(cell_idx)
                            else:
                                valid = False
                                break
                        if valid:
                            cols.append(W * H * D + piece_idx)
                            row_id = f"Piece{piece_idx}_R{rot}_T({tx},{ty},{tz})"
                            matrix_rows.append((row_id, cols))
                            
    return headers, matrix_rows

if __name__ == "__main__":
    print("Homeward Offline DLX Level Solver initialized.")
    # Example test grid: 2x2x1 with 2 1x2 pieces
    grid_size = (2, 2, 1)
    pieces = [
        [(0,0,0), (1,0,0)],
        [(0,0,0), (0,1,0)],
    ]
    headers, rows = generate_matrix_for_level(grid_size, pieces)
    print(f"Generated {len(rows)} possible placements for {len(pieces)} pieces.")
    
    results = []
    header_node = create_dlx_matrix(headers, rows)
    search(header_node, 0, [], results)
    
    if results:
        print("Level is solvable! Solution:")
        for r in results[0]:
            print("  ", r)
    else:
        print("Level is NOT solvable!")
