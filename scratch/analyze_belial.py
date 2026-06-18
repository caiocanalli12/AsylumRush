from PIL import Image
import numpy as np

# Load belial.png
try:
    belial = Image.open("resources/imagens/belial.png").convert("RGBA")
    print(f"=== belial.png ===")
    print(f"Size: {belial.size}")
    print(f"Mode: {belial.mode}")
    
    belial_arr = np.array(belial)
    
    # Let's find rows with non-transparent content
    row_starts = []
    row_ends = []
    prev_had_content = False
    
    for y in range(belial.size[1]):
        # Check if there is any pixel with alpha > 10 in this row
        content_count = np.sum(belial_arr[y, :, 3] > 10)
        has_content = content_count > 2 # more than 2 pixels of content
        
        if has_content and not prev_had_content:
            row_starts.append(y)
            prev_had_content = True
        elif not has_content and prev_had_content:
            row_ends.append(y - 1)
            prev_had_content = False
            
    if prev_had_content:
        row_ends.append(belial.size[1] - 1)
        
    print(f"Content rows found: {len(row_starts)}")
    for i, (rs, re) in enumerate(zip(row_starts, row_ends)):
        print(f"Row {i}: y range [{rs}, {re}], height = {re - rs + 1}")
        
        # Now find column blocks for this row
        in_block = False
        block_start = 0
        col_blocks = []
        
        for x in range(belial.size[0]):
            # Check if there is any pixel with alpha > 10 in this column within the row height
            has_col_content = np.any(belial_arr[rs:re+1, x, 3] > 10)
            
            if has_col_content and not in_block:
                block_start = x
                in_block = True
            elif not has_col_content and in_block:
                col_blocks.append((block_start, x - 1))
                in_block = False
                
        if in_block:
            col_blocks.append((block_start, belial.size[0] - 1))
            
        print(f"  Frames found: {len(col_blocks)}")
        for j, (cs, ce) in enumerate(col_blocks):
            w = ce - cs + 1
            h = re - rs + 1
            # Let's also crop slightly to remove empty margins if desired,
            # but getting the raw bounding box is a great start.
            print(f"    Frame {j}: x={cs}, y={rs}, w={w}, h={h} -> Rectangle: {{ {cs}, {rs}, {w}, {h} }}")
            
except Exception as e:
    print("Error analyzing belial.png:", e)
