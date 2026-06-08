from PIL import Image

img = Image.open("resources/imagens/polarbear.png").convert("RGBA")
width, height = img.size
pixels = img.load()

rows = []
in_row = False
row_start = 0

for y in range(height):
    has_pixels = any(pixels[x, y][3] > 0 for x in range(width))
    if has_pixels and not in_row:
        in_row = True
        row_start = y
    elif not has_pixels and in_row:
        in_row = False
        rows.append((row_start, y - 1))

if in_row:
    rows.append((row_start, height - 1))

for i, (ry1, ry2) in enumerate(rows):
    sprites = []
    in_sprite = False
    sprite_start = 0
    for x in range(width):
        has_pixels = any(pixels[x, y][3] > 0 for y in range(ry1, ry2 + 1))
        if has_pixels and not in_sprite:
            in_sprite = True
            sprite_start = x
        elif not has_pixels and in_sprite:
            in_sprite = False
            sx1 = sprite_start
            sx2 = x - 1
            
            tight_y1 = ry2
            tight_y2 = ry1
            for sy in range(ry1, ry2 + 1):
                for sx in range(sx1, sx2 + 1):
                    if pixels[sx, sy][3] > 0:
                        tight_y1 = min(tight_y1, sy)
                        tight_y2 = max(tight_y2, sy)
            
            sprites.append((sx1, tight_y1, sx2 - sx1 + 1, tight_y2 - tight_y1 + 1))
            
    print(f"Linha {i+1} tem {len(sprites)} frames:")
    for j, (x, y, w, h) in enumerate(sprites):
        print(f"  Col {j+1}: {{ {x}, {y}, {w}, {h} }}")
