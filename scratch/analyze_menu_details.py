from PIL import Image
img = Image.open("resources/imagens/fundo/menu.png")
w, h = img.size
# Let's check non-gray pixels
# A pixel is "gray background" if R, G, B are all very close to each other and in the range [210, 222]
non_gray_samples = []
for y in range(0, h, 10): # sample every 10 rows
    for x in range(0, w, 10): # sample every 10 cols
        r, g, b = img.getpixel((x, y))
        # check if it's NOT the background light gray
        is_bg = (210 <= r <= 225) and (210 <= g <= 225) and (210 <= b <= 225) and abs(r-g) < 5 and abs(g-b) < 5 and abs(r-b) < 5
        if not is_bg:
            non_gray_samples.append((x, y, (r, g, b)))

print("Total non-gray samples:", len(non_gray_samples))
print("First 30 samples:")
for sample in non_gray_samples[:30]:
    print(sample)
