from PIL import Image

img = Image.open("resources/imagens/fundo/ifsp_highschool.png")
w, h = img.size

# Let's print the colors along the Y axis at X = 100
print("Vertical line color profile at X=100:")
for y in range(0, h, 5):
    color = img.getpixel((100, y))
    print(f"Y={y:3d}: {color}")
