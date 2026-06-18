import zlib
import struct

def paeth_predictor(a, b, c):
    p = a + b - c
    pa = abs(p - a)
    pb = abs(p - b)
    pc = abs(p - c)
    if pa <= pb and pa <= pc:
        return a
    elif pb <= pc:
        return b
    else:
        return c

def parse_png_rgba(path):
    with open(path, 'rb') as f:
        sig = f.read(8)
        if sig != b'\x89PNG\r\n\x1a\n':
            raise ValueError("Not a valid PNG file")
        
        chunks = []
        idat_data = b""
        width, height = 0, 0
        bit_depth, color_type = 0, 0
        
        while True:
            length_bytes = f.read(4)
            if not length_bytes or len(length_bytes) < 4:
                break
            length = struct.unpack('>I', length_bytes)[0]
            chunk_type = f.read(4)
            chunk_data = f.read(length)
            f.read(4) # CRC
            
            if chunk_type == b'IHDR':
                width, height, bit_depth, color_type, _, _, _ = struct.unpack('>IIBBBBB', chunk_data)
                if color_type != 6 or bit_depth != 8:
                    raise ValueError(f"Only RGBA 8-bit PNGs are supported (color_type={color_type}, bit_depth={bit_depth})")
            elif chunk_type == b'IDAT':
                idat_data += chunk_data
            elif chunk_type == b'IEND':
                break
                
        # Decompress pixel data
        decompressed = zlib.decompress(idat_data)
        
        # Defilter scanlines
        bpp = 4 # RGBA
        stride = 1 + width * bpp
        pixels = bytearray(width * height * bpp)
        
        prior_line = bytearray(width * bpp)
        current_line = bytearray(width * bpp)
        
        for y in range(height):
            scanline = decompressed[y * stride : (y + 1) * stride]
            filter_type = scanline[0]
            raw_data = scanline[1:]
            
            # Defilter
            if filter_type == 0: # None
                current_line = bytearray(raw_data)
            elif filter_type == 1: # Sub
                for x in range(width * bpp):
                    left = current_line[x - bpp] if x >= bpp else 0
                    current_line[x] = (raw_data[x] + left) & 0xFF
            elif filter_type == 2: # Up
                for x in range(width * bpp):
                    up = prior_line[x]
                    current_line[x] = (raw_data[x] + up) & 0xFF
            elif filter_type == 3: # Average
                for x in range(width * bpp):
                    left = current_line[x - bpp] if x >= bpp else 0
                    up = prior_line[x]
                    current_line[x] = (raw_data[x] + (left + up) // 2) & 0xFF
            elif filter_type == 4: # Paeth
                for x in range(width * bpp):
                    left = current_line[x - bpp] if x >= bpp else 0
                    up = prior_line[x]
                    left_up = prior_line[x - bpp] if x >= bpp else 0
                    pred = paeth_predictor(left, up, left_up)
                    current_line[x] = (raw_data[x] + pred) & 0xFF
            else:
                raise ValueError(f"Unknown PNG filter type: {filter_type}")
                
            # Copy to pixels array
            pixels[y * width * bpp : (y + 1) * width * bpp] = current_line
            prior_line = bytearray(current_line)
            
        return pixels, width, height

def analyze_belial():
    pixels, w, h = parse_png_rgba('resources/imagens/belial.png')
    bpp = 4
    
    # Analyze rows
    row_has_content = []
    for y in range(h):
        has_content = False
        for x in range(w):
            alpha = pixels[(y * w + x) * bpp + 3]
            if alpha > 10:
                has_content = True
                break
        row_has_content.append(has_content)
        
    row_starts = []
    row_ends = []
    prev_had_content = False
    
    for y in range(h):
        has_content = row_has_content[y]
        if has_content and not prev_had_content:
            row_starts.append(y)
            prev_had_content = True
        elif not has_content and prev_had_content:
            row_ends.append(y - 1)
            prev_had_content = False
            
    if prev_had_content:
        row_ends.append(h - 1)
        
    print(f"=== belial.png Analysis ===")
    print(f"Size: {w}x{h}")
    print(f"Content rows found: {len(row_starts)}")
    
    for i, (rs, re) in enumerate(zip(row_starts, row_ends)):
        print(f"Row {i}: y range [{rs}, {re}], height = {re - rs + 1}")
        
        # Analyze columns
        col_blocks = []
        in_block = False
        block_start = 0
        
        for x in range(w):
            has_col_content = False
            for y in range(rs, re + 1):
                alpha = pixels[(y * w + x) * bpp + 3]
                if alpha > 10:
                    has_col_content = True
                    break
                    
            if has_col_content and not in_block:
                block_start = x
                in_block = True
            elif not has_col_content and in_block:
                col_blocks.append((block_start, x - 1))
                in_block = False
                
        if in_block:
            col_blocks.append((block_start, w - 1))
            
        print(f"  Frames found: {len(col_blocks)}")
        for j, (cs, ce) in enumerate(col_blocks):
            frame_w = ce - cs + 1
            frame_h = re - rs + 1
            print(f"    Frame {j}: x={cs}, y={rs}, w={frame_w}, h={frame_h} -> {{ {cs}, {rs}, {frame_w}, {frame_h} }}")

if __name__ == '__main__':
    analyze_belial()
