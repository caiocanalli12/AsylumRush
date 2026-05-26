from PIL import Image, ImageDraw, ImageFont

# Load the background image
img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Crop the region of interest: x_game from 1900 to 3800
# Since x_game corresponds to x_img - 10, we crop x_img from 1910 to 3810.
# We also include the top margin (y_img from 30 to 313, which is y_game from 0 to 283)
x_start = 1910
x_end = 3810
y_start = 30
y_end = 313

cropped = img.crop((x_start, y_start, x_end, y_end))

# Create a drawing context on the cropped image
draw = ImageDraw.Draw(cropped)

# Draw grid lines and labels every 50 pixels (relative to game coordinates)
# Game coord x corresponds to cropped image x = x_game - (x_start - 10) = x_game - 1900
for x_game in range(1900, 3801, 50):
    x_crop = x_game - 1900
    # Draw vertical line
    draw.line([(x_crop, 0), (x_crop, y_end - y_start)], fill=(255, 0, 0), width=1)
    # Draw text label at the top
    draw.text((x_crop + 2, 5), str(x_game), fill=(255, 255, 255))

for y_game in range(0, 284, 20):
    y_crop = y_game
    # Draw horizontal line
    draw.line([(0, y_crop), (x_end - x_start, y_crop)], fill=(0, 0, 255), width=1)
    # Draw text label
    draw.text((5, y_crop + 2), str(y_game), fill=(255, 255, 255))

# Save the grid image to artifacts
cropped.save("C:/Users/Admin/.gemini/antigravity/brain/93129e43-2a13-496d-9043-33378fe1d267/playground_grid.png")
print("Grid drawn and saved successfully!")
