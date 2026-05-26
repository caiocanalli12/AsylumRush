#include <stdio.h>
#include "raylib.h"

int main() {
    Image img = LoadImage("resources/imagens/fundo/frozensuburbs.png");
    if (img.data == NULL) {
        printf("Failed to load image!\n");
        return 1;
    }
    
    // Let's print out the pixels around the diagonal line of both fences.
    // Fence 1 starts around x_game = 2210, y_game = 180.
    // Let's see: where does the diagonal line end?
    // Let's look at the bottom-most dark pixels.
    int end_x1 = -1;
    for (int x = 2200; x < 2500; x++) {
        for (int y = 200; y < 283; y++) {
            Color c = GetImageColor(img, x + 10, y + 30);
            if (c.r < 30 && c.g < 30 && c.b < 30 && c.a > 200) {
                if (x > end_x1) end_x1 = x;
            }
        }
    }
    printf("Fence 1 maximum X with dark pixels: %d\n", end_x1);

    int end_x2 = -1;
    for (int x = 3300; x < 3800; x++) {
        for (int y = 200; y < 283; y++) {
            Color c = GetImageColor(img, x + 10, y + 30);
            if (c.r < 30 && c.g < 30 && c.b < 30 && c.a > 200) {
                if (x > end_x2) end_x2 = x;
            }
        }
    }
    printf("Fence 2 maximum X with dark pixels: %d\n", end_x2);

    UnloadImage(img);
    return 0;
}
