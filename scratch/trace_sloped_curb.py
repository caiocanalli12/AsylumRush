from PIL import Image

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Let's inspect the color profile in columns 8300, 8350, 8400, 8450, 8500, 8550, 8600
# from Y=175 to Y=260 to locate the street curb.
# The street curb is a black/dark line or transition from wall to pavement.

for x in range(8300, 8601, 50):
    print(f"\n=== Column x = {x} ===")
    x_img = x + 10
    for y in range(175, 261, 2):
        y_img = y + 30
        r, g, b, *a = img.getpixel((x_img, y_img))
        print(f"y={y}: R={r}, G={g}, B={b}")
