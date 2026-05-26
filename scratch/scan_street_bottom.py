from PIL import Image

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

print("Tracing pavement Y boundaries (x_game = 8200 to 9000)...")

for x_game in range(8200, 9001, 50):
    x_img = x_game + 10
    pavement_ys = []
    for y_game in range(140, 283):
        y_img = y_game + 30
        r, g, b, *a = img.getpixel((x_img, y_img))
        
        # Pavement colors: dark blue/grey, e.g. R=20, G=33, B=54
        # Or the paint line: R=51, G=114, B=167
        # Or R=42, G=43, B=57
        # Let's write a flexible condition:
        is_pavement = (r < 55 and g < 55 and b < 65) or (r == 51 and g == 114 and b == 167)
        if is_pavement:
            pavement_ys.append(y_game)
            
    if pavement_ys:
        print(f"x={x_game} -> Y min={min(pavement_ys)}, Y max={max(pavement_ys)}, height={max(pavement_ys) - min(pavement_ys) + 1}")
    else:
        print(f"x={x_game} -> No pavement found!")
