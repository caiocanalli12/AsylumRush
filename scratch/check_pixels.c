#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Check");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    Color bg = {16, 112, 132, 255};
    
    // Let's print out the colors for x in [104, 115], y in [432, 440]
    for (int y = 430; y <= 444; y++) {
        for (int x = 100; x <= 120; x++) {
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
