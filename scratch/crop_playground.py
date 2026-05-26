from PIL import Image

# Load the frozensuburbs image
img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Define the region we want to crop (in original image coordinates)
# We want game coords: x = 1950 to 2550, y = 100 to 283
# Image coords: x = 1950 + 10, y = 100 + 30, w = 600, h = 183
box = (1960, 130, 2560, 313)
cropped = img.crop(box)

# Save to the artifacts directory
cropped.save("C:/Users/Admin/.gemini/antigravity/brain/93129e43-2a13-496d-9043-33378fe1d267/playground_fence.png")
print("Cropped successfully!")
