from PIL import Image
import collections

for idx in range(6):
    img = Image.open(f"scratch/block_{idx}.png")
    w, h = img.size
    non_trans = []
    for y in range(h):
        for x in range(w):
            r, g, b, a = img.getpixel((x, y))
            if a > 0:
                non_trans.append((r, g, b))
    counter = collections.Counter(non_trans)
    print(f"Block {idx}: size={img.size}, non-transparent pixels={len(non_trans)}")
    print("  Most common non-transparent colors:")
    for color, count in counter.most_common(3):
        print(f"    Color: {color}, Count: {count}")
