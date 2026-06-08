from PIL import Image

img = Image.open("resources/imagens/wolf_dummy.png")
width, height = img.size
pixels = img.load()

# Find non-transparent pixels
active_pixels = []
for y in range(height):
    for x in range(width):
        if pixels[x, y][3] > 0: # Alpha > 0
            active_pixels.append((x, y))

# Group pixels into contiguous blobs (sprites)
# A simple approach: 
# Find horizontal slices that have pixels, then within those slices find vertical slices.
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

print(f"Found {len(rows)} rows.")

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
            
            # Find exact tight bounds for this specific sprite
            sx1 = sprite_start
            sx2 = x - 1
            
            # Find tight Y bounds within the row
            tight_y1 = ry2
            tight_y2 = ry1
            for sy in range(ry1, ry2 + 1):
                for sx in range(sx1, sx2 + 1):
                    if pixels[sx, sy][3] > 0:
                        tight_y1 = min(tight_y1, sy)
                        tight_y2 = max(tight_y2, sy)
            
            sprites.append((sx1, tight_y1, sx2 - sx1 + 1, tight_y2 - tight_y1 + 1))
            
    print(f"\nRow {i} tem {len(sprites)} frames:")
    rects_str = ", ".join([f"{{ {x}, {y}, {w}, {h} }}" for (x, y, w, h) in sprites])
    print(f"static const Rectangle row{i}_frames[{len(sprites)}] = {{ {rects_str} }};")

