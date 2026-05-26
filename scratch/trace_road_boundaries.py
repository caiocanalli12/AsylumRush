from PIL import Image

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Let's trace the pavement boundaries.
# For each x_game from 8200 to 9343:
# We map to x_img = x_game + 10.
# We scan y_game from 140 to 283.
# We identify the pavement by its color:
# Let's count a pixel as pavement if it is dark blue/grey:
# (R < 55 and G < 55 and B < 65) or (R == 51 and G == 114 and B == 167)
# But wait, there might be other colors on the pavement like grey/snow.
# Let's print out the pavement Y range for every 10 pixels.

print("Tracing street pavement Y bounds:")
for x_game in range(8200, 9344, 20):
    x_img = x_game + 10
    pavement_ys = []
    for y_game in range(140, 283):
        y_img = y_game + 30
        r, g, b, *a = img.getpixel((x_img, y_img))
        
        # Pavement check:
        # Asphalt is very dark, R < 45, G < 45, B < 65
        # Paint lines: R=51, G=114, B=167
        is_pavement = (r < 45 and g < 45 and b < 65) or (r == 51 and g == 114 and b == 167)
        
        # But we must exclude the mezzanine/sidewalk.
        # The mezzanine Y range is [140, 180]. 
        # So we only consider street pavement starting from Y >= 180.
        if y_game >= 180 and is_pavement:
            pavement_ys.append(y_game)
            
    if pavement_ys:
        # Check if the pavement is continuous or has gaps
        print(f"x={x_game} -> Y range: [{min(pavement_ys)}, {max(pavement_ys)}], width={max(pavement_ys) - min(pavement_ys) + 1}")
    else:
        print(f"x={x_game} -> No pavement found (Y >= 180)")
