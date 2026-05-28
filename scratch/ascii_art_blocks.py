from PIL import Image

for idx in range(1, 6):
    img = Image.open(f"scratch/block_{idx}.png")
    # downsample to 80x15
    small = img.resize((80, 15), Image.Resampling.NEAREST)
    pixels = small.load()
    w, h = small.size
    print(f"\n--- BLOCK {idx} ---")
    for y in range(h):
        line = ""
        for x in range(w):
            r, g, b, a = pixels[x, y]
            if a > 0:
                line += "#"
            else:
                line += " "
        print(line)
