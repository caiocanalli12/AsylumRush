from analyze_belial_row1 import parse_png_rgba

def inspect_block():
    pixels, w, h = parse_png_rgba('resources/imagens/belial.png')
    bpp = 4
    
    # Let's inspect colors in x=400..559, y=47..248
    colors = {}
    for y in range(47, h):
        for x in range(390, w):
            idx = (y * w + x) * bpp
            r, g, b, a = pixels[idx:idx+4]
            if a > 0:
                color = (r, g, b, a)
                colors[color] = colors.get(color, 0) + 1
                
    print("Colors in the right block (x>=390, y>=47):")
    sorted_colors = sorted(colors.items(), key=lambda item: item[1], reverse=True)
    for color, count in sorted_colors[:10]:
        print(f"  Color {color}: {count} pixels")

if __name__ == '__main__':
    inspect_block()
