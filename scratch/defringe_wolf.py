from PIL import Image
import numpy as np

img = Image.open("resources/imagens/wolf_dummy.png").convert("RGBA")
arr = np.array(img)

R = arr[:,:,0].astype(int)
G = arr[:,:,1].astype(int)
B = arr[:,:,2].astype(int)

# Background masks
bg_mask = (G > 150) & (R < 100) & (B < 100)
dark_mask = (R < 20) & (G < 20) & (B < 20)

arr[bg_mask, 3] = 0
arr[dark_mask, 3] = 0

# Defringe: where G is dominant, cap it to the max of R and B
# This neutralizes the green halo into a gray/black halo which blends perfectly
greenish = (G > R) & (G > B) & ~bg_mask & ~dark_mask
max_rb = np.maximum(R, B)
arr[greenish, 1] = max_rb[greenish]

# Create cleaned image
cleaned = Image.fromarray(arr)

# Upscale by 4x to increase resolution
new_size = (cleaned.width * 4, cleaned.height * 4)
# LANCZOS provides a smoothed "HD" look compared to nearest neighbor
scaled = cleaned.resize(new_size, Image.Resampling.LANCZOS)

# Post-process after Lanczos: some transparent pixels might have become semi-transparent with weird colors
# We can just save it
scaled.save("resources/imagens/wolf_dummy.png")
print(f"Processado: tamanho original {cleaned.size}, novo tamanho {scaled.size}")
