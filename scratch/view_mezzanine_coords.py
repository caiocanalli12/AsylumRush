from PIL import Image, ImageDraw, ImageFont

# Load the background image
img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Crop the region of interest around the mezzanine
# x_game from 8200 to 9450 -> x_img = x_game + 10
# y_game from 0 to 283 -> y_img = y_game + 30
x_start = 8200 + 10
x_end = 9450 + 10
y_start = 0 + 30
y_end = 283 + 30

cropped = img.crop((x_start, y_start, x_end, y_end))

# Draw grid lines and labels every 10 pixels horizontally and vertically
draw = ImageDraw.Draw(cropped)

# Horizontal lines
for y_game in range(0, 284, 10):
    y_crop = y_game
    color = (0, 0, 255) if y_game % 50 == 0 else (100, 100, 255)
    draw.line([(0, y_crop), (x_end - x_start, y_crop)], fill=color, width=1)
    if y_game % 20 == 0:
        draw.text((5, y_crop + 1), str(y_game), fill=(255, 255, 255))

# Vertical lines
for x_game in range(8200, 9451, 10):
    x_crop = x_game - 8200
    color = (255, 0, 0) if x_game % 50 == 0 else (255, 100, 100)
    draw.line([(x_crop, 0), (x_crop, y_end - y_start)], fill=color, width=1)
    if x_game % 50 == 0:
        draw.text((x_crop + 2, 5), str(x_game), fill=(255, 255, 255))

# Save the grid image to artifacts
cropped.save("C:/Users/Admin/.gemini/antigravity/brain/93129e43-2a13-496d-9043-33378fe1d267/mezzanine_fine_grid.png")
print("Fine grid drawn and saved to C:/Users/Admin/.gemini/antigravity/brain/93129e43-2a13-496d-9043-33378fe1d267/mezzanine_fine_grid.png")
