from analyze_belial_row1 import parse_png_rgba

def find_gaps():
    pixels, w, h = parse_png_rgba('resources/imagens/belial.png')
    bpp = 4
    chroma_color1 = (0, 184, 240)
    chroma_color2 = (255, 255, 255)
    
    # Check if a row has content
    row_has_content = []
    for y in range(h):
        has_content = False
        for x in range(w):
            idx = (y * w + x) * bpp
            r, g, b, a = pixels[idx:idx+4]
            is_chroma = (r == chroma_color1[0] and g == chroma_color1[1] and b == chroma_color1[2]) or \
                        (r == chroma_color2[0] and g == chroma_color2[1] and b == chroma_color2[2])
            if a > 10 and not is_chroma:
                has_content = True
                break
        row_has_content.append(has_content)
        
    # Find all continuous ranges of rows with content
    ranges = []
    start = None
    for y in range(h):
        if row_has_content[y]:
            if start is None:
                start = y
        else:
            if start is not None:
                ranges.append((start, y - 1))
                start = None
    if start is not None:
        ranges.append((start, h - 1))
        
    print("Horizontal ranges with content (gaps separate them):")
    for r in ranges:
        print(f"  y range [{r[0]}, {r[1]}], height = {r[1] - r[0] + 1}")

if __name__ == '__main__':
    find_gaps()
