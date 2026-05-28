import shutil
from PIL import Image

# Backup original
try:
    shutil.copy("resources/imagens/fundo/menu.png", "resources/imagens/fundo/menu_original_backup.png")
    print("Backed up original menu.png")
except Exception as e:
    print("Backup error:", e)

# Read original
img = Image.open("resources/imagens/fundo/menu.png")
img = img.convert("RGBA")
pixels = img.load()
w, h = img.size

transparent_count = 0
for y in range(h):
    for x in range(w):
        r, g, b, a = pixels[x, y]
        # Any color close to light gray [210, 223]
        is_bg = (210 <= r <= 223) and (210 <= g <= 223) and (210 <= b <= 223) and abs(r - g) <= 3 and abs(g - b) <= 3 and abs(r - b) <= 3
        if is_bg:
            pixels[x, y] = (0, 0, 0, 0)
            transparent_count += 1

print(f"Made transparent: {transparent_count} pixels ({transparent_count / (w*h) * 100:.2f}%)")
img.save("resources/imagens/fundo/menu.png")
print("Successfully overwrote menu.png with the transparent version!")
