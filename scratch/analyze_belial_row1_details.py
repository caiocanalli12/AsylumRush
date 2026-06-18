from analyze_belial_row1 import parse_png_rgba

def analyze_row1_details():
    pixels, w, h = parse_png_rgba('resources/imagens/belial.png')
    bpp = 4
    rs, re = 47, 190
    
    # Let's count non-transparent (and non-chroma) pixels per column in this region
    chroma_color = (0, 184, 240)
    col_has_content = []
    
    for x in range(w):
        col_content = False
        for y in range(rs, re + 1):
            idx = (y * w + x) * bpp
            r, g, b, a = pixels[idx:idx+4]
            is_chroma = (r == chroma_color[0] and g == chroma_color[1] and b == chroma_color[2])
            if a > 10 and not is_chroma:
                col_content = True
                break
        col_has_content.append(col_content)
        
    print("Content state of columns 1 to 396 (showing continuous blocks of content/empty):")
    blocks = []
    in_block = col_has_content[1]
    start = 1
    for x in range(1, 397):
        if col_has_content[x] != in_block:
            blocks.append((in_block, start, x - 1))
            start = x
            in_block = col_has_content[x]
    blocks.append((in_block, start, 396))
    
    for state, s, e in blocks:
        print(f"  Columns {s} to {e}: has_content = {state} (width = {e - s + 1})")

if __name__ == '__main__':
    analyze_row1_details()
