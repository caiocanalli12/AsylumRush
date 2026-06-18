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
                    raise ValueError(f"Only RGBA 8-bit PNGs are supported")
            elif chunk_type == b'IDAT':
                idat_data += chunk_data
            elif chunk_type == b'IEND':
                break
                
        decompressed = zlib.decompress(idat_data)
        bpp = 4 # RGBA
        stride = 1 + width * bpp
        pixels = bytearray(width * height * bpp)
        prior_line = bytearray(width * bpp)
        current_line = bytearray(width * bpp)
        
        for y in range(height):
            scanline = decompressed[y * stride : (y + 1) * stride]
            filter_type = scanline[0]
            raw_data = scanline[1:]
            
            if filter_type == 0:
                current_line = bytearray(raw_data)
            elif filter_type == 1:
                for x in range(width * bpp):
                    left = current_line[x - bpp] if x >= bpp else 0
                    current_line[x] = (raw_data[x] + left) & 0xFF
            elif filter_type == 2:
                for x in range(width * bpp):
                    up = prior_line[x]
                    current_line[x] = (raw_data[x] + up) & 0xFF
            elif filter_type == 3:
                for x in range(width * bpp):
                    left = current_line[x - bpp] if x >= bpp else 0
                    up = prior_line[x]
                    current_line[x] = (raw_data[x] + (left + up) // 2) & 0xFF
            elif filter_type == 4:
                for x in range(width * bpp):
                    left = current_line[x - bpp] if x >= bpp else 0
                    up = prior_line[x]
                    left_up = prior_line[x - bpp] if x >= bpp else 0
                    pred = paeth_predictor(left, up, left_up)
                    current_line[x] = (raw_data[x] + pred) & 0xFF
            
            pixels[y * width * bpp : (y + 1) * width * bpp] = current_line
            prior_line = bytearray(current_line)
            
        return pixels, width, height

def analyze_row1():
    pixels, w, h = parse_png_rgba('resources/imagens/belial.png')
    bpp = 4
    
    rs, re = 47, 190
    
    col_counts = []
    for x in range(w):
        count = 0
        for y in range(rs, re + 1):
            alpha = pixels[(y * w + x) * bpp + 3]
            if alpha > 0:
                count += 1
        col_counts.append(count)
        
    print("Column content counts for x up to 410:")
    zero_cols = []
    for x in range(0, 410):
        if col_counts[x] == 0:
            zero_cols.append(x)
            
    print("Zero columns in range 0-410:")
    if not zero_cols:
        print("None!")
    else:
        # Group into ranges
        ranges = []
        start = zero_cols[0]
        prev = zero_cols[0]
        for x in zero_cols[1:]:
            if x == prev + 1:
                prev = x
            else:
                ranges.append((start, prev))
                start = x
                prev = x
        ranges.append((start, prev))
        print(ranges)

if __name__ == '__main__':
    analyze_row1()
