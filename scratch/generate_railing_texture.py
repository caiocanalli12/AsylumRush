from PIL import Image

# Load the background
img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Railing region:
# Crop Y only up to 176 (excluding the sidewalk floor and black curb line at 180+)
x_start = 8300 + 10
x_end = 9343 + 10
y_start = 130 + 30
y_end = 176 + 30

cropped = img.crop((x_start, y_start, x_end, y_end)).convert("RGBA")
width, height = cropped.size

# Process pixels to isolate the dark railing structures
for x in range(width):
    for y in range(height):
        r, g, b, a = cropped.getpixel((x, y))
        
        # Railing filter:
        is_black = (r == 0 and g == 0 and b == 0)
        is_slate = (25 <= r <= 45 and 35 <= g <= 55 and 45 <= b <= 60)
        is_dark_railing = (r < 25 and g < 33 and b < 50 and (r + g + b) > 0)
        
        if is_black or is_slate or is_dark_railing:
            cropped.putpixel((x, y), (r, g, b, 255))
        else:
            cropped.putpixel((x, y), (0, 0, 0, 0))

# Save the railing texture
cropped.save("resources/imagens/fundo/mezzanine_railing.png")
print("Saved isolated railing texture to resources/imagens/fundo/mezzanine_railing.png")

# Save debug version overlaid on red background
debug_img = Image.new("RGBA", (width, height), (255, 0, 0, 255))
debug_img.alpha_composite(cropped)
debug_img.save("C:/Users/Admin/.gemini/antigravity/brain/93129e43-2a13-496d-9043-33378fe1d267/railing_debug.png")
print("Saved debug verification image to railing_debug.png")
