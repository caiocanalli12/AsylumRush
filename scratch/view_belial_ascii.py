from analyze_belial_row1 import parse_png_rgba

def draw_ascii():
    pixels, w, h = parse_png_rgba('resources/imagens/belial.png')
    bpp = 4
    chroma_color = (0, 184, 240)
    
    # Target height and width for ASCII
    target_w = 120
    target_h = 40
    
    scale_x = w / target_w
    scale_y = h / target_h
    
    print("=== ASCII ART OF belial.png ===")
    for ty in range(target_h):
        line = ""
        y_start = int(ty * scale_y)
        y_end = int((ty + 1) * scale_y)
        if y_end == y_start:
            y_end = y_start + 1
            
        for tx in range(target_w):
            x_start = int(tx * scale_x)
            x_end = int((tx + 1) * scale_x)
            if x_end == x_start:
                x_end = x_start + 1
                
            # Count solid vs transparent pixels in this cell
            solid = 0
            total = 0
            for y in range(y_start, min(y_end, h)):
                for x in range(x_start, min(x_end, w)):
                    idx = (y * w + x) * bpp
                    r, g, b, a = pixels[idx:idx+4]
                    is_chroma = (r == chroma_color[0] and g == chroma_color[1] and b == chroma_color[2])
                    if a > 10 and not is_chroma:
                        solid += 1
                    total += 1
            
            if total > 0 and solid / total > 0.15:
                line += "#"
            else:
                line += "."
        print(line)

if __name__ == '__main__':
    draw_ascii()
