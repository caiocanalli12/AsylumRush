from PIL import Image
import collections

img = Image.open("resources/imagens/fundo/menu.png")
print("Size:", img.size)
print("Mode:", img.mode)

# Let's count the most common colors to see which is the background color
# We can sample a subset or count all
pixels = list(img.getdata())
counter = collections.Counter(pixels)
print("Top 10 most common colors:")
for color, count in counter.most_common(10):
    print(f"Color: {color}, Count: {count}")
