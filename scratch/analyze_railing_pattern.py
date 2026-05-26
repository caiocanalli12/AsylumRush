from PIL import Image

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

start_x = 8300
end_x = 9343

# Detect vertical bars by finding columns with high dark pixel count
columns = []
for x in range(start_x, end_x + 1):
    x_img = x + 10
    dark_count = 0
    for y in range(130, 185):
        y_img = y + 30
        r, g, b, *a = img.getpixel((x_img, y_img))
        if r < 80 and g < 80 and b < 80:
            dark_count += 1
    columns.append((x, dark_count))

# Find the threshold for a vertical bar.
# Let's list columns with count > 30 (high probability of being a bar or post)
bar_cols = [x for x, count in columns if count >= 28]

# Calculate the distance between consecutive bars
print("Detected vertical bars at X coordinates:")
print(bar_cols[:30], "... (total", len(bar_cols), "columns)")

gaps = []
for i in range(len(bar_cols) - 1):
    dist = bar_cols[i+1] - bar_cols[i]
    if dist > 15: # typical spacing is probably around 10-20px, let's look for larger gaps
        gaps.append((bar_cols[i], bar_cols[i+1], dist))

print("\nLarge gaps between vertical bars:")
for left, right, dist in gaps:
    print(f"Gap between x={left} and x={right} (distance = {dist}px)")
