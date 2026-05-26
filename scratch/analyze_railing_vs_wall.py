from PIL import Image

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Let's inspect some columns around the gap area (x=8850 to x=8950)
# We know x=8897 has a vertical bar (dark post), and x=8920 is in the gap (only background wall).
# Let's print out the Y coordinates and colors for both!

print("=== Column X=8897 (Railing Post) ===")
x1_img = 8897 + 10
for y in range(130, 186):
    y_img = y + 30
    r, g, b, *a = img.getpixel((x1_img, y_img))
    if r < 100 or g < 100 or b < 100:
        print(f"y={y}: R={r}, G={g}, B={b}")

print("\n=== Column X=8920 (Wall only, inside gap) ===")
x2_img = 8920 + 10
for y in range(130, 186):
    y_img = y + 30
    r, g, b, *a = img.getpixel((x2_img, y_img))
    if r < 100 or g < 100 or b < 100:
        print(f"y={y}: R={r}, G={g}, B={b}")
