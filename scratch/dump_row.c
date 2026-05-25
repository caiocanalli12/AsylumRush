#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Dump");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    
    // Grid background color is {16, 112, 132, 255} or {65, 169, 184, 255}
    // Let's print ASCII representation of Y=430..445, X=90..280
    for (int y = 430; y <= 445; y++) {
        printf("%3d: ", y);
        for (int x = 95; x <= 280; x++) {
            Color c = GetImageColor(img, x, y);
            bool isBg = (c.r == 16 && c.g == 112 && c.b == 132) || (c.r == 65 && c.g == 169 && c.b == 184);
            if (isBg) {
                printf(".");
            } else if (c.r == 252 && c.g == 252 && c.b == 252) {
                printf("#");
            } else if (c.r == 0 && c.g == 0 && c.b == 0) {
                printf("@");
            } else {
                printf("?");
            }
        }
        printf("\n");
    }
    
    UnloadImage(img);
    CloseWindow();
    return 0;
}
