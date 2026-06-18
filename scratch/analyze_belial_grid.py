from analyze_belial_row1 import parse_png_rgba

def analyze_grid():
    pixels, w, h = parse_png_rgba('resources/imagens/belial.png')
    bpp = 4
    
    # Let's count unique colors (ignoring fully transparent pixels)
    colors = {}
    for y in range(h):
        for x in range(w):
            idx = (y * w + x) * bpp
            r, g, b, a = pixels[idx:idx+4]
            if a > 0:
                color = (r, g, b, a)
                colors[color] = colors.get(color, 0) + 1
                
    # Sort colors by frequency
    sorted_colors = sorted(colors.items(), key=lambda item: item[1], reverse=True)
    print("Most frequent non-transparent colors:")
    for color, count in sorted_colors[:15]:
        print(f"  Color {color}: {count} pixels")

    # Let's count non-transparent pixels per column in Row 1 (y=47..190)
    rs, re = 47, 190
    print("\nNon-zero pixel counts for columns in Row 1 (filtering count < 20):")
    cols_below_threshold = []
    for x in range(w):
        count = 0
        for y in range(rs, re + 1):
            alpha = pixels[(y * w + x) * bpp + 3]
            if alpha > 10:
                count += 1
        if count < 20 and count > 0:
            print(f"  Col {x}: {count} pixels")

if __name__ == '__main__':
    analyze_grid()
