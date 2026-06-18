from analyze_belial_row1 import parse_png_rgba

def find_sprites_floodfill():
    pixels, w, h = parse_png_rgba('resources/imagens/belial.png')
    bpp = 4
    chroma_color1 = (0, 184, 240)
    chroma_color2 = (255, 255, 255)
    
    # Mark walkable non-transparent pixels
    visited = [[False for _ in range(w)] for _ in range(h)]
    is_solid = [[False for _ in range(w)] for _ in range(h)]
    
    for y in range(h):
        for x in range(w):
            idx = (y * w + x) * bpp
            r, g, b, a = pixels[idx:idx+4]
            is_chroma = (r == chroma_color1[0] and g == chroma_color1[1] and b == chroma_color1[2]) or \
                        (r == chroma_color2[0] and g == chroma_color2[1] and b == chroma_color2[2])
            if a > 10 and not is_chroma:
                is_solid[y][x] = True
                
    sprites = []
    
    # Flood fill to find connected components
    for y in range(h):
        for x in range(w):
            if is_solid[y][x] and not visited[y][x]:
                # Found a new component, flood fill it
                queue = [(x, y)]
                visited[y][x] = True
                
                min_x, max_x = x, x
                min_y, max_y = y, y
                
                head = 0
                while head < len(queue):
                    cx, cy = queue[head]
                    head += 1
                    
                    min_x = min(min_x, cx)
                    max_x = max(max_x, cx)
                    min_y = min(min_y, cy)
                    max_y = max(max_y, cy)
                    
                    # Check 8 neighbors
                    for dx in [-1, 0, 1]:
                        for dy in [-1, 0, 1]:
                            nx, ny = cx + dx, cy + dy
                            if 0 <= nx < w and 0 <= ny < h:
                                if is_solid[ny][nx] and not visited[ny][nx]:
                                    visited[ny][nx] = True
                                    queue.append((nx, ny))
                                    
                # Save component if it's large enough (ignore tiny noise)
                comp_w = max_x - min_x + 1
                comp_h = max_y - min_y + 1
                if comp_w > 5 and comp_h > 5:
                    sprites.append((min_x, min_y, comp_w, comp_h, len(queue)))
                    
    # Sort sprites: primary by Y coordinate (row), secondary by X coordinate (column)
    # Since y coordinate can vary slightly, we group sprites that have close Y centers into rows
    sprites.sort(key=lambda s: (s[1] + s[3]/2, s[0]))
    
    print("Individual sprites found via flood-fill:")
    for i, (sx, sy, sw, sh, size) in enumerate(sprites):
        print(f"  Sprite {i}: x={sx}, y={sy}, w={sw}, h={sh} (pixels={size}) -> {{ {sx}, {sy}, {sw}, {sh} }}")

if __name__ == '__main__':
    find_sprites_floodfill()
