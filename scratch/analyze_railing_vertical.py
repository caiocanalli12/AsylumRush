from PIL import Image

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Let's find the vertical bounds of the railing.
# We scan x_game from 8300 to 9343.
# For each column, we look for dark pixels of the railing (R < 80, G < 80, B < 80).
# We print the minimum and maximum Y where these dark pixels occur.

all_ys = []
for x in range(8300, 9343):
    x_img = x + 10
    for y in range(130, 190):
        y_img = y + 30
        r, g, b, *a = img.getpixel((x_img, y_img))
        if r < 80 and g < 80 and b < 80:
            all_ys.append(y)

if all_ys:
    print(f"Railing Y range: min={min(all_ys)}, max={max(all_ys)}")
else:
    print("No railing pixels found!")
