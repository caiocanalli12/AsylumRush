from PIL import Image

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

for x_game in range(8300, 8701, 50):
    print(f"\n--- Column X = {x_game} ---")
    x_img = x_game + 10
    for y_game in range(210, 284, 2):
        y_img = y_game + 30
        r, g, b, *a = img.getpixel((x_img, y_img))
        print(f"y={y_game}: R={r}, G={g}, B={b}")
