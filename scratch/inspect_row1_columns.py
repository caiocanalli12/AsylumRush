from analyze_belial_row1 import parse_png_rgba

def inspect_cols():
    pixels, w, h = parse_png_rgba('resources/imagens/belial.png')
    bpp = 4
    chroma_color1 = (0, 184, 240)
    chroma_color2 = (255, 255, 255)
    
    # We want to print for each column x in [390..559], which y coordinates contain content
    print("Content coordinates for columns >= 390:")
    for x in range(390, w):
        y_with_content = []
        for y in range(h):
            idx = (y * w + x) * bpp
            r, g, b, a = pixels[idx:idx+4]
            is_chroma = (r == chroma_color1[0] and g == chroma_color1[1] and b == chroma_color1[2]) or \
                        (r == chroma_color2[0] and g == chroma_color2[1] and b == chroma_color2[2])
            if a > 10 and not is_chroma:
                y_with_content.append(y)
        if y_with_content:
            # Group into ranges
            ranges = []
            start = y_with_content[0]
            prev = y_with_content[0]
            for y in y_with_content[1:]:
                if y == prev + 1:
                    prev = y
                else:
                    ranges.append((start, prev))
                    start = y
                    prev = y
            ranges.append((start, prev))
            print(f"  Col {x}: {ranges}")

if __name__ == '__main__':
    inspect_cols()
