from PIL import Image
import collections

img = Image.open("resources/imagens/fundo/menu.png")
pixels = list(img.getdata())
counter = collections.Counter(pixels)

print("Colors with more than 500 pixels:")
sorted_colors = sorted(counter.items(), key=lambda x: x[1], reverse=True)
for color, count in sorted_colors:
    if count > 500:
        print(f"Color: {color}, Count: {count}")
    else:
        break
