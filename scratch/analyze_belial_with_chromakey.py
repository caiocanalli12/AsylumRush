from analyze_belial_row1 import parse_png_rgba

def analyze_with_chromakey():
    pixels, w, h = parse_png_rgba('resources/imagens/belial.png')
    bpp = 4
    
    chroma_color = (0, 184, 240)
    
    # Process pixels: make chroma color transparent
    processed_alpha = []
    for y in range(h):
        row_alpha = []
        for x in range(w):
            idx = (y * w + x) * bpp
            r, g, b, a = pixels[idx:idx+4]
            # Check if it's chroma key
            is_chroma = (r == chroma_color[0] and g == chroma_color[1] and b == chroma_color[2])
            if is_chroma or a <= 10:
                row_alpha.append(0)
            else:
                row_alpha.append(a)
        processed_alpha.append(row_alpha)
        
    # Find rows with content
    row_starts = []
    row_ends = []
    prev_had_content = False
    
    for y in range(h):
        # Row has content if there is at least one non-transparent pixel
        has_content = any(val > 0 for val in processed_alpha[y])
        if has_content and not prev_had_content:
            row_starts.append(y)
            prev_had_content = True
        elif not has_content and prev_had_content:
            row_ends.append(y - 1)
            prev_had_content = False
            
    if prev_had_content:
        row_ends.append(h - 1)
        
    print(f"=== belial.png Analysis with Chroma Key ===")
    print(f"Size: {w}x{h}")
    print(f"Content rows found: {len(row_starts)}")
    
    for i, (rs, re) in enumerate(zip(row_starts, row_ends)):
        print(f"Row {i}: y range [{rs}, {re}], height = {re - rs + 1}")
        
        # Analyze columns
        col_blocks = []
        in_block = False
        block_start = 0
        
        for x in range(w):
            has_col_content = False
            for y in range(rs, re + 1):
                if processed_alpha[y][x] > 0:
                    has_col_content = True
                    break
                    
            if has_col_content and not in_block:
                block_start = x
                in_block = True
            elif not has_col_content and in_block:
                col_blocks.append((block_start, x - 1))
                in_block = False
                
        if in_block:
            col_blocks.append((block_start, w - 1))
            
        print(f"  Frames found: {len(col_blocks)}")
        for j, (cs, ce) in enumerate(col_blocks):
            frame_w = ce - cs + 1
            frame_h = re - rs + 1
            print(f"    Frame {j}: x={cs}, y={rs}, w={frame_w}, h={frame_h} -> {{ {cs}, {rs}, {frame_w}, {frame_h} }}")

if __name__ == '__main__':
    analyze_with_chromakey()
