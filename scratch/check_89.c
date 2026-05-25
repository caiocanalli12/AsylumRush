#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Check89");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    Color bg = {16, 112, 132, 255};
    
    // Print non-bg pixels at Y=433..444 for X=228..270
    for (int y = 433; y <= 444; y++) {
        for (int x = 228; x <= 270; x++) {
            Color c = GetImageColor(img, x, y);
            if (c.r != bg.r || c.g != bg.g || c.b != bg.b) {
                printf("Pixel at (%d, %d): R:%d G:%d B:%d A:%d\n", x, y, c.r, c.g, c.b, c.a);
            }
        }
    }
    
    UnloadImage(img);
    CloseWindow();
    return 0;
}
