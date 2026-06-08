from PIL import Image
import numpy as np

wolf = Image.open("resources/imagens/wolf_dummy.png").convert("RGBA")
wolf_arr = np.array(wolf)
w, h = wolf.size

def is_bg(r, g, b, a):
    if a < 10:
        return True
    if r < 50 and g > 200 and b < 50:  # green
        return True
    if r < 40 and g < 40 and b < 40:   # dark/black
        return True
    return False

# Find row boundaries more carefully - look for horizontal separator lines
print("Finding row boundaries...")
row_content = []
for y in range(h):
    count = 0
    for x in range(w):
        r, g, b, a = wolf_arr[y, x]
        if not is_bg(r, g, b, a):
            count += 1
    row_content.append(count)

# Identify row blocks
rows = []
in_block = False
block_start = 0
for y in range(h):
    if row_content[y] > 3 and not in_block:
        block_start = y
        in_block = True
    elif row_content[y] <= 3 and in_block:
        rows.append((block_start, y - 1))
        in_block = False
if in_block:
    rows.append((block_start, h - 1))

print(f"Found {len(rows)} animation rows:")
for i, (rs, re) in enumerate(rows):
    print(f"  Row {i}: y=[{rs},{re}], height={re-rs+1}")

# For each row, find individual frames by vertical gaps
for i, (rs, re) in enumerate(rows):
    col_content = []
    for x in range(w):
        count = 0
        for y in range(rs, re + 1):
            r, g, b, a = wolf_arr[y, x]
            if not is_bg(r, g, b, a):
                count += 1
        col_content.append(count)
    
    frames = []
    in_frame = False
    frame_start = 0
    for x in range(w):
        if col_content[x] > 0 and not in_frame:
            frame_start = x
            in_frame = True
        elif col_content[x] == 0 and in_frame:
            frames.append((frame_start, x - 1))
            in_frame = False
    if in_frame:
        frames.append((frame_start, w - 1))
    
    print(f"\n  Row {i} (y={rs}-{re}): {len(frames)} frames")
    for j, (cs, ce) in enumerate(frames):
        print(f"    Frame {j}: x=[{cs},{ce}], size=({ce-cs+1}x{re-rs+1})")
