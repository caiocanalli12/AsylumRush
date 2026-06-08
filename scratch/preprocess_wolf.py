from PIL import Image
import numpy as np

# Load wolf_dummy.png and convert green chroma key to transparency
img = Image.open("resources/imagens/wolf_dummy.png").convert("RGBA")
arr = np.array(img)

# Green chroma key: R < 80, G > 180, B < 80
green_mask = (arr[:,:,0] < 80) & (arr[:,:,1] > 180) & (arr[:,:,2] < 80)

# Also handle near-black background areas (dark cells in spritesheet)
dark_mask = (arr[:,:,0] < 15) & (arr[:,:,1] < 15) & (arr[:,:,2] < 15)

# Set alpha to 0 for green and dark background pixels
arr[green_mask, 3] = 0
arr[dark_mask, 3] = 0

result = Image.fromarray(arr)
result.save("resources/imagens/wolf_dummy.png")
print(f"Processed wolf_dummy.png: {result.size}, green pixels removed: {green_mask.sum()}, dark pixels removed: {dark_mask.sum()}")
