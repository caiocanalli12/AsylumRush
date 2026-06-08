from PIL import Image

img = Image.open("resources/imagens/EarDog.png").convert("RGBA")
width, height = img.size
pixels = img.load()

# Find pink pixels
pink_pixels = []
for y in range(height):
    for x in range(width):
        r, g, b, a = pixels[x, y]
        if r > 200 and g < 100 and b > 150 and a > 0:
            pink_pixels.append((x, y))

# Group into bounding boxes
if pink_pixels:
    min_x = min(p[0] for p in pink_pixels)
    max_x = max(p[0] for p in pink_pixels)
    min_y = min(p[1] for p in pink_pixels)
    max_y = max(p[1] for p in pink_pixels)
    
    # Let's find all contiguous heads or just find the one that matches ln4col4
    # Actually, let's just print the rough locations of pink clusters
    clusters = []
    for x, y in pink_pixels:
        found = False
        for c in clusters:
            cx, cy = c[0]
            if abs(x - cx) < 50 and abs(y - cy) < 50:
                c.append((x, y))
                found = True
                break
        if not found:
            clusters.append([(x, y)])
            
    for i, c in enumerate(clusters):
        cx_min = min(p[0] for p in c)
        cx_max = max(p[0] for p in c)
        cy_min = min(p[1] for p in c)
        cy_max = max(p[1] for p in c)
        # expand downwards to include the head (head is below the bow)
        head_y_max = cy_max + 30 # rough guess
        
        # let's find all non-transparent pixels in this expanded box
        hx_min = cx_min - 20
        hx_max = cx_max + 20
        hy_min = cy_min
        hy_max = head_y_max
        
        real_hx_min, real_hx_max = width, 0
        real_hy_min, real_hy_max = height, 0
        
        for y in range(max(0, hy_min), min(height, hy_max + 20)):
            for x in range(max(0, hx_min), min(width, hx_max)):
                if pixels[x, y][3] > 0:
                    real_hx_min = min(real_hx_min, x)
                    real_hx_max = max(real_hx_max, x)
                    real_hy_min = min(real_hy_min, y)
                    real_hy_max = max(real_hy_max, y)
                    
        print(f"Cluster {i}: Bow at ({cx_min},{cy_min}) to ({cx_max},{cy_max}). Head Rect: {{ {real_hx_min}, {real_hy_min}, {real_hx_max - real_hx_min}, {real_hy_max - real_hy_min} }}")
