from PIL import Image, ImageDraw

def find_sprites():
    img = Image.open("resources/imagens/EarDog.png")
    width, height = img.size
    print(f"Image size: {width}x{height}")
    
    # We want to find contiguous or grid-like blocks of non-transparent pixels.
    # Alternatively, we can draw a grid of 50x50, 100x100 or something to inspect visually.
    # Let's draw a grid of 50x50 and save it first.
    grid_img = img.copy()
    draw = ImageDraw.Draw(grid_img)
    
    for x in range(0, width, 50):
        draw.line([(x, 0), (x, height)], fill=(255, 0, 0), width=1)
        draw.text((x + 2, 2), str(x), fill=(255, 255, 0))
        
    for y in range(0, height, 50):
        draw.line([(0, y), (width, y)], fill=(0, 255, 0), width=1)
        draw.text((2, y + 2), str(y), fill=(255, 255, 0))
        
    grid_img.save("C:/Users/jdavi/.gemini/antigravity/brain/e4289673-aaea-4e49-9e3d-ae0f34d66562/eardog_grid.png")
    print("Saved grid image to artifacts.")

if __name__ == "__main__":
    find_sprites()
