from PIL import Image

img = Image.open("resources/imagens/fundo/menu_transparent.png")
w, h = img.size
pixels = img.load()

# Find the bounding box of all non-transparent pixels
min_x, min_y = w, h
max_x, max_y = -1, -1

for y in range(h):
    for x in range(w):
        r, g, b, a = pixels[x, y]
        if a > 0:
            if x < min_x: min_x = x
            if x > max_x: max_x = x
            if y < min_y: min_y = y
            if y > max_y: max_y = y

print(f"Overall bounding box of content: X: [{min_x}, {max_x}], Y: [{min_y}, {max_y}]")

# Let's divide the image into vertical blocks and see where the density is
row_density = [0] * h
for y in range(h):
    for x in range(w):
        if pixels[x, y][3] > 0:
            row_density[y] += 1

# Let's find ranges of Y where density is > 0
in_block = False
start_y = -1
blocks = []
for y in range(h):
    if row_density[y] > 5 and not in_block:
        in_block = True
        start_y = y
    elif row_density[y] <= 5 and in_block:
        in_block = False
        blocks.append((start_y, y - 1))
if in_block:
    blocks.append((start_y, h - 1))

print("Vertical blocks of content:")
for idx, (sy, ey) in enumerate(blocks):
    # Find horizontal bounds for this block
    bx_min = w
    bx_max = -1
    for y in range(sy, ey + 1):
        for x in range(w):
            if pixels[x, y][3] > 0:
                if x < bx_min: bx_min = x
                if x > bx_max: bx_max = x
    print(f"Block {idx}: Y: [{sy}, {ey}], X: [{bx_min}, {bx_max}], Height: {ey - sy + 1}, Width: {bx_max - bx_min + 1}")
