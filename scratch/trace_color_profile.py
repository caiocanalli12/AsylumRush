from PIL import Image

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Let's write the map to a file
with open("c:/Users/Admin/OneDrive/Arquivos IFSP/Módulo 5/ORIN/PlatformGame/scratch/slope_map.txt", "w") as f:
    f.write("Slope Map: x from 8250 to 8650, y from 160 to 283\n")
    f.write("Legend: # = Street pavement, . = Wall/other, = = Paint/curb line\n\n")
    
    # We print column headers
    f.write("      ")
    for x in range(8250, 8651, 10):
        f.write(f"{x//10:03d} ")
    f.write("\n")
    
    for y in range(160, 284):
        f.write(f"y={y:03d} ")
        for x in range(8250, 8651, 10):
            x_img = x + 10
            y_img = y + 30
            r, g, b, *a = img.getpixel((x_img, y_img))
            
            # Identify street pavement color
            is_pavement = (r < 45 and g < 45 and b < 65)
            is_line = (r == 51 and g == 114 and b == 167) or (r > 150 and g > 150 and b > 100) or (r == 0 and g == 0 and b == 0)
            
            if is_pavement:
                char = "#"
            elif is_line:
                char = "="
            else:
                char = "."
            f.write(f" {char}  ")
        f.write("\n")

print("Generated slope map at scratch/slope_map.txt")
