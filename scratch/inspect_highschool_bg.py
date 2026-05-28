from PIL import Image

img = Image.open("resources/imagens/fundo/ifsp_highschool.png")
print("Size:", img.size)
w, h = img.size

# Let's check colors in the last few rows
for y in range(h - 10, h):
    row_colors = [img.getpixel((x, y)) for x in range(0, w, w // 10)]
    print(f"Row {y}: {row_colors[:5]}")
