from PIL import Image

img = Image.open("resources/imagens/fundo/mezzanine_railing.png")
width, height = img.size

solid_count = 0
transparent_count = 0

for x in range(width):
    for y in range(height):
        r, g, b, a = img.getpixel((x, y))
        if a > 0:
            solid_count += 1
        else:
            transparent_count += 1

total = width * height
pct_solid = (solid_count / total) * 100
print(f"Mezzanine Railing verification:")
print(f"Total pixels: {total}, Solid: {solid_count} ({pct_solid:.2f}%), Transparent: {transparent_count}")
