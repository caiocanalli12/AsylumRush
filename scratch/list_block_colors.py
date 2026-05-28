from PIL import Image
import collections

img = Image.open("scratch/block_1.png")
pixels = list(img.getdata())
counter = collections.Counter(pixels)

print("Colors in block_1.png with > 10 pixels:")
sorted_colors = sorted(counter.items(), key=lambda x: x[1], reverse=True)
for color, count in sorted_colors:
    if count > 10:
        print(f"Color: {color}, Count: {count}")
