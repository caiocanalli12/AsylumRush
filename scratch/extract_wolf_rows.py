from PIL import Image

img = Image.open("resources/imagens/wolf_dummy.png")
rows = [
  (5, 37), (49, 88), (107, 137), (147, 182), (201, 237), (259, 301), (314, 354)
]
for i, (rs, re) in enumerate(rows):
    cropped = img.crop((0, rs, img.width, re))
    cropped.save(f"scratch/wolf_row_{i}.png")
