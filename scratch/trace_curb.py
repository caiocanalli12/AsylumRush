from PIL import Image

img = Image.open("resources/imagens/fundo/frozensuburbs.png")

# Let's trace the curb y coordinate for x_game from 8280 to 8600.
# We map x_game to x_img = x_game + 10.
# We start y_game at 250 (inside the street asphalt) and walk upwards.
# The street asphalt color is very dark blue/grey: R=20, G=33, B=54.
# Sometimes there are paint stripes (lighter blue) or snow/dirt.
# But generally, the curb boundary is a black/dark line (R=0,G=0,B=0 or R<20,G<20,B<20)
# above which the texture changes to the concrete wall/sidewalk (brownish/greyish with R > 40, G > 40, B > 40).
# Let's find the first pixel from the top that is street asphalt (where R < 30, G < 40, B < 65)
# or let's print the transition from street to wall.

curb_y = {}
for x in range(8280, 8620):
    x_img = x + 10
    
    # We find the transition by scanning from y_game = 170 to 230
    found_y = 220 # default fallback
    for y in range(170, 230):
        y_img = y + 30
        r, g, b, *a = img.getpixel((x_img, y_img))
        
        # The wall/sidewalk/bridge above the street is brownish/yellowish or concrete
        # Let's check if we transition to the dark street pavement.
        # Let's check the pavement color: typically R ~ 20, G ~ 33, B ~ 54.
        # Or R ~ 42, G ~ 43, B ~ 57.
        # But the curb itself has a black border.
        # Let's detect the transition:
        # A pixel (r, g, b) is wall if r > 50 and g > 45 and (b is not very blue)
        # Pavement has r < 35, g < 40, b < 65.
        # Let's trace the bottom-most non-street pixel, or the top-most street pixel.
        is_street = (r < 35 and g < 45 and b < 65) or (r == 51 and g == 114 and b == 167)
        if is_street:
            found_y = y
            break
    curb_y[x] = found_y

# Print key points
print("Curb Y coordinates:")
for x in range(8280, 8620, 10):
    print(f"x={x} -> y={curb_y.get(x)}")
