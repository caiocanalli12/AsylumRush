from find_belial_sprites_floodfill import find_sprites_floodfill
from analyze_belial_row1 import parse_png_rgba

def group_sprites():
    pixels, w, h = parse_png_rgba('resources/imagens/belial.png')
    bpp = 4
    chroma_color1 = (0, 184, 240)
    chroma_color2 = (255, 255, 255)
    
    # Run flood fill but keep only sprites with > 150 pixels
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
    for y in range(h):
        for x in range(w):
            if is_solid[y][x] and not visited[y][x]:
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
                    for dx in [-1, 0, 1]:
                        for dy in [-1, 0, 1]:
                            nx, ny = cx + dx, cy + dy
                            if 0 <= nx < w and 0 <= ny < h:
                                if is_solid[ny][nx] and not visited[ny][nx]:
                                    visited[ny][nx] = True
                                    queue.append((nx, ny))
                comp_w = max_x - min_x + 1
                comp_h = max_y - min_y + 1
                if comp_w > 15 and comp_h > 15 and len(queue) > 150:
                    sprites.append((min_x, min_y, comp_w, comp_h, len(queue)))
                    
    # Group by Y coordinate (within tolerance of 15px)
    rows = []
    for s in sprites:
        added = False
        sy_center = s[1] + s[3]/2
        for r in rows:
            r_center = sum(item[1] + item[3]/2 for item in r) / len(r)
            if abs(sy_center - r_center) < 15:
                r.append(s)
                added = True
                break
        if not added:
            rows.append([s])
            
    # Sort rows by their average Y coordinate
    rows.sort(key=lambda r: sum(item[1] + item[3]/2 for item in r) / len(r))
    
    # Print grouped rows
    for i, r in enumerate(rows):
        r.sort(key=lambda s: s[0]) # sort sprites in row by X coordinate
        avg_y = sum(item[1] + item[3]/2 for item in r) / len(r)
        print(f"Animation Row {i} (Average Y Center: {avg_y:.1f}):")
        for j, s in enumerate(r):
            print(f"  Frame {j}: x={s[0]}, y={s[1]}, w={s[2]}, h={s[3]} (pixels={s[4]}) -> {{ {s[0]}, {s[1]}, {s[2]}, {s[3]} }}")

if __name__ == '__main__':
    group_sprites()
