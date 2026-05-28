from PIL import Image

img = Image.open("resources/imagens/fundo/menu_transparent.png")

# Bounding boxes from the previous run
blocks = [
    (15, 473, 364, 1367), # Y: [15, 473], X: [364, 1367]
    (495, 566, 598, 1070),
    (573, 644, 598, 1070),
    (651, 721, 598, 1070),
    (729, 800, 598, 1070),
    (808, 879, 598, 1070),
]

for idx, (sy, ey, sx, ex) in enumerate(blocks):
    cropped = img.crop((sx, sy, ex, ey))
    cropped.save(f"scratch/block_{idx}.png")
    print(f"Saved scratch/block_{idx}.png")
