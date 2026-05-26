from PIL import Image, ImageDraw

# Load the background image
img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Crop the region around the first fence: game coordinates x = 2000 to 2700, y = 100 to 283
# Image coordinates: x = 2010 to 2710, y = 130 to 313
x_start = 2010
x_end = 2710
y_start = 30
y_end = 313

cropped = img.crop((x_start, y_start, x_end, y_end))
draw = ImageDraw.Draw(cropped)

# Draw vertical lines and labels every 10 pixels
for x_game in range(2000, 2701, 10):
    x_crop = x_game - 2000
    # Draw line: green for 50s, red for 100s, gray for 10s
    if x_game % 100 == 0:
        color = (255, 0, 0)
        width = 2
    elif x_game % 50 == 0:
        color = (0, 255, 0)
        width = 1.5
    else:
        color = (128, 128, 128)
        width = 1
        
    draw.line([(x_crop, 0), (x_crop, y_end - y_start)], fill=color, width=int(width))
    
    # Label every 50 pixels
    if x_game % 50 == 0:
        draw.text((x_crop + 2, 5), str(x_game), fill=(255, 255, 255))

# Draw horizontal lines every 10 pixels
for y_game in range(100, 284, 10):
    y_crop = y_game
    if y_game % 50 == 0:
        color = (0, 0, 255)
        width = 2
    else:
        color = (128, 128, 128)
        width = 1
    draw.line([(0, y_crop), (x_end - x_start, y_crop)], fill=color, width=int(width))
    if y_game % 50 == 0 or y_game % 10 == 0:
        draw.text((5, y_crop + 2), str(y_game), fill=(255, 255, 255))

# Save the grid image to artifacts
cropped.save("C:/Users/Admin/.gemini/antigravity/brain/93129e43-2a13-496d-9043-33378fe1d267/first_fence_fine_grid.png")
print("Fine grid drawn and saved successfully!")
