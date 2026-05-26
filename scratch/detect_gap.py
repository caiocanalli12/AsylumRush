from PIL import Image

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

start_x = 8300
end_x = 9343

# We count the number of dark pixels in the Y range [130, 185]
gap_profile = []
for x in range(start_x, end_x + 1):
    x_img = x + 10
    dark_count = 0
    for y in range(130, 185):
        y_img = y + 30
        r, g, b, *a = img.getpixel((x_img, y_img))
        # Let's count pixels that are dark (outlines/fence bars are black/dark grey)
        if r < 80 and g < 80 and b < 80:
            dark_count += 1
    gap_profile.append((x, dark_count))

# Let's find the minimum and maximum dark counts
min_count = min(count for x, count in gap_profile)
max_count = max(count for x, count in gap_profile)
print(f"Min dark count: {min_count}, Max dark count: {max_count}")

# Let's print out the x ranges where the dark count is close to the minimum
low_threshold = min_count + 3
current_run = []
runs = []
for x, count in gap_profile:
    if count <= low_threshold:
        current_run.append((x, count))
    else:
        if current_run:
            runs.append(current_run)
            current_run = []
if current_run:
    runs.append(current_run)

print("Ranges of low dark pixel density:")
for run in runs:
    if len(run) >= 10:
        start_x = run[0][0]
        end_x = run[-1][0]
        avg_count = sum(c for x, c in run) / len(run)
        print(f"Low density from x={start_x} to x={end_x} (width {end_x - start_x + 1}px, avg count={avg_count:.2f})")
