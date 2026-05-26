#include <stdio.h>
#include "raylib.h"

int main() {
    Image img = LoadImage("resources/imagens/fundo/frozensuburbs.png");
    if (img.data == NULL) {
        printf("Failed to load image!\n");
        return 1;
    }
    
    // Fence 1: around x_game = 2100 to 2600
    // Fence 2: around x_game = 3200 to 3700
    // Let's print out where the black fence pixels are!
    // The fence consists of vertical metal bars with spikes.
    // Let's look at y_game in [180, 283].
    // Let's print out intervals of x where there are spikes.
    
    printf("Scanning Fence 1 (x: 2100 - 2600):\n");
    for (int x = 2100; x < 2600; x += 5) {
        int dark_count = 0;
        for (int y = 180; y < 283; y++) {
            Color c = GetImageColor(img, x + 10, y + 30);
            if (c.r < 15 && c.g < 15 && c.b < 15 && c.a > 200) {
                dark_count++;
            }
        }
        if (dark_count > 5) {
            printf("x=%d: dark_count=%d\n", x, dark_count);
        }
    }
    
    printf("\nScanning Fence 2 (x: 3200 - 3700):\n");
    for (int x = 3200; x < 3700; x += 5) {
        int dark_count = 0;
        for (int y = 180; y < 283; y++) {
            Color c = GetImageColor(img, x + 10, y + 30);
            if (c.r < 15 && c.g < 15 && c.b < 15 && c.a > 200) {
                dark_count++;
            }
        }
        if (dark_count > 5) {
            printf("x=%d: dark_count=%d\n", x, dark_count);
        }
    }

    UnloadImage(img);
    return 0;
}
