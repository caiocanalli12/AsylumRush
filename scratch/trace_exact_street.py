from PIL import Image

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

print("Tracing yellow lines on the street (x_game = 8200 to 9400)...")

for x_game in range(8200, 9401, 20):
    x_img = x_game + 10
    yellow_ys = []
    for y_game in range(150, 283):
        y_img = y_game + 30
        r, g, b, *a = img.getpixel((x_img, y_img))
        
        # Yellow color check: high red and green, low blue
        if r > 180 and g > 150 and b < 100:
            yellow_ys.append(y_game)
            
    if yellow_ys:
        print(f"x={x_game} -> Yellow Y: {yellow_ys}")
