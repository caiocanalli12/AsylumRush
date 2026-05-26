from PIL import Image, ImageDraw

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Crop the region x_game = 8800 to 9050, y_game = 100 to 220
# x_img = x_game + 10, y_img = y_game + 30
x_start = 8800 + 10
x_end = 9050 + 10
y_start = 100 + 30
y_end = 220 + 30

cropped = img.crop((x_start, y_start, x_end, y_end))

# Let's draw a grid every 10 pixels to see details clearly
draw = ImageDraw.Draw(cropped)
for x in range(8800, 9051, 10):
    x_crop = x - 8800
    color = (255, 0, 0) if x % 50 == 0 else (255, 150, 150)
    draw.line([(x_crop, 0), (x_crop, y_end - y_start)], fill=color, width=1)
    if x % 20 == 0:
        draw.text((x_crop + 2, 5), str(x), fill=(255, 255, 255))

for y in range(100, 221, 10):
    y_crop = y - 100
    color = (0, 0, 255) if y % 50 == 0 else (150, 150, 255)
    draw.line([(0, y_crop), (x_end - x_start, y_crop)], fill=color, width=1)
    draw.text((5, y_crop + 1), str(y), fill=(255, 255, 255))

cropped.save("C:/Users/Admin/.gemini/antigravity/brain/93129e43-2a13-496d-9043-33378fe1d267/gap_slice_grid.png")
print("Saved slice to gap_slice_grid.png")
