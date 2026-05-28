from PIL import Image

img = Image.open("resources/imagens/fundo/menu.png")
img = img.convert("RGBA")
pixels = img.load()
w, h = img.size

transparent_count = 0
kept_count = 0

for y in range(h):
    for x in range(w):
        r, g, b, a = pixels[x, y]
        # check if it's the light gray background
        # Let's check if R, G, B are in range [210, 222] and very close to each other
        is_bg = (210 <= r <= 223) and (210 <= g <= 223) and (210 <= b <= 223) and abs(r - g) <= 3 and abs(g - b) <= 3 and abs(r - b) <= 3
        if is_bg:
            pixels[x, y] = (0, 0, 0, 0)
            transparent_count += 1
        else:
            kept_count += 1

print(f"Made transparent: {transparent_count} pixels ({transparent_count / (w*h) * 100:.2f}%)")
print(f"Kept: {kept_count} pixels ({kept_count / (w*h) * 100:.2f}%)")

img.save("resources/imagens/fundo/menu_transparent.png")
print("Saved resources/imagens/fundo/menu_transparent.png")
