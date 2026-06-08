from PIL import Image
import numpy as np

# Analyze wolf_dummy.png
wolf = Image.open("resources/imagens/wolf_dummy.png").convert("RGBA")
print(f"=== wolf_dummy.png ===")
print(f"Size: {wolf.size}")
print(f"Mode: {wolf.mode}")

# The spritesheet has green (0,255,0) background. Let's find frame boundaries.
wolf_arr = np.array(wolf)
# Green background: R < 50, G > 200, B < 50
green_mask = (wolf_arr[:,:,0] < 50) & (wolf_arr[:,:,1] > 200) & (wolf_arr[:,:,2] < 50)
# Also check for dark/black background
dark_mask = (wolf_arr[:,:,0] < 30) & (wolf_arr[:,:,1] < 30) & (wolf_arr[:,:,2] < 30) & (wolf_arr[:,:,3] > 200)

# Find rows that are entirely green or dark (row separators)
row_seps = []
for y in range(wolf.size[1]):
    row = wolf_arr[y, :, :]
    is_sep = True
    for x in range(wolf.size[0]):
        r, g, b, a = row[x]
        if a < 10:
            continue  # transparent
        if (r < 50 and g > 200 and b < 50):  # green
            continue
        if (r < 40 and g < 40 and b < 40):  # dark
            continue
        is_sep = False
        break
    if not is_sep:
        pass

# Let's try a different approach - scan for horizontal green lines
print("\nRow-by-row content analysis (non-bg pixel count):")
prev_had_content = False
row_starts = []
for y in range(wolf.size[1]):
    content_count = 0
    for x in range(wolf.size[0]):
        r, g, b, a = wolf_arr[y, x]
        # Not green bg, not black bg, not transparent
        if a > 10 and not (r < 50 and g > 200 and b < 50) and not (r < 40 and g < 40 and b < 40):
            content_count += 1
    has_content = content_count > 5
    if has_content and not prev_had_content:
        row_starts.append(y)
    prev_had_content = has_content

print(f"Content row start positions: {row_starts}")
print(f"Number of animation rows: {len(row_starts)}")

# Now let's find column separators for each row
# Let's look for vertical green lines
print("\nColumn analysis (vertical green separator positions):")
col_seps = []
for x in range(wolf.size[0]):
    content_count = 0
    for y in range(wolf.size[1]):
        r, g, b, a = wolf_arr[y, x]
        if a > 10 and not (r < 50 and g > 200 and b < 50) and not (r < 40 and g < 40 and b < 40):
            content_count += 1
    if content_count == 0:
        col_seps.append(x)

# Find continuous ranges of separator columns
if col_seps:
    sep_ranges = []
    start = col_seps[0]
    prev = col_seps[0]
    for x in col_seps[1:]:
        if x == prev + 1:
            prev = x
        else:
            sep_ranges.append((start, prev))
            start = x
            prev = x
    sep_ranges.append((start, prev))
    print(f"Vertical separator ranges (first 20): {sep_ranges[:20]}")

print("\n\n=== ice_shard.png ===")
ice = Image.open("resources/imagens/ice_shard.png").convert("RGBA")
print(f"Size: {ice.size}")
print(f"Mode: {ice.mode}")

# Find non-transparent bounding boxes
ice_arr = np.array(ice)
print("\nRow-by-row content analysis:")
prev_had_content = False
ice_row_starts = []
ice_row_ends = []
for y in range(ice.size[1]):
    content_count = 0
    for x in range(ice.size[0]):
        r, g, b, a = ice_arr[y, x]
        if a > 10:
            content_count += 1
    has_content = content_count > 3
    if has_content and not prev_had_content:
        ice_row_starts.append(y)
    if not has_content and prev_had_content:
        ice_row_ends.append(y - 1)
    prev_had_content = has_content
if prev_had_content:
    ice_row_ends.append(ice.size[1] - 1)

print(f"Content row ranges: {list(zip(ice_row_starts, ice_row_ends))}")

# For each content row, find column blocks
for i, (rs, re) in enumerate(zip(ice_row_starts, ice_row_ends)):
    print(f"\nRow {i} (y={rs}-{re}):")
    # Find columns with content
    col_blocks = []
    in_block = False
    block_start = 0
    for x in range(ice.size[0]):
        has_content = False
        for y in range(rs, re + 1):
            if ice_arr[y, x, 3] > 10:
                has_content = True
                break
        if has_content and not in_block:
            block_start = x
            in_block = True
        elif not has_content and in_block:
            col_blocks.append((block_start, x - 1))
            in_block = False
    if in_block:
        col_blocks.append((block_start, ice.size[0] - 1))
    print(f"  Column blocks: {col_blocks}")
    for j, (cs, ce) in enumerate(col_blocks):
        print(f"    Frame {j}: x=[{cs},{ce}], y=[{rs},{re}], size=({ce-cs+1}x{re-rs+1})")
