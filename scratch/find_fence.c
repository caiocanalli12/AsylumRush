#include <stdio.h>
#include "raylib.h"

int main() {
    Image img = LoadImage("resources/imagens/fundo/frozensuburbs.png");
    if (img.data == NULL) {
        printf("Failed to load image!\n");
        return 1;
    }
    
    // We want to find the fence/pillar.
    // The stone pillar has brick colors and black borders.
    // Let's scan along y_game = 180 (curb line, y_img = 210) for x_game in [0, 10603]
    // and find where there are black pixels (r=0, g=0, b=0) or very dark vertical structures.
    // Let's scan a grid around y_game = 180 to 220.
    
    printf("Scanning for vertical dark structures (pillar/fence)...\n");
    for (int x = 0; x < 10603; x += 10) {
        int dark_count = 0;
        for (int y = 170; y < 220; y++) {
            Color c = GetImageColor(img, x + 10, y + 30);
            if (c.r < 10 && c.g < 10 && c.b < 10 && c.a > 200) {
                dark_count++;
            }
        }
        if (dark_count > 15) {
            printf("Dark vertical column at x_game = %d (dark count: %d)\n", x, dark_count);
        }
    }

    UnloadImage(img);
    return 0;
}
