from PIL import Image, ImageFilter

print("Opening frozensuburbs.png...")
img = Image.open("resources/imagens/fundo/frozensuburbs.png")
print("Applying Gaussian Blur...")
# Apply a nice blur to the background
blurred = img.filter(ImageFilter.GaussianBlur(radius=8))
print("Saving blurred image...")
blurred.save("resources/imagens/fundo/frozensuburbs_blurred.png")
print("Done!")
