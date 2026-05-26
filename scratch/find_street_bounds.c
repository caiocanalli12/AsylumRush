#include <stdio.h>
#include "raylib.h"

int main() {
    Image img = LoadImage("resources/imagens/fundo/frozensuburbs.png");
    if (img.data == NULL) {
        printf("Failed to load image!\n");
        return 1;
    }
    
    // Let's print out the pixel colors along x_game = 150 for y_game in [160, 283].
    // This will help us determine the exact range where the sidewalk/street is.
    printf("Vertical color profile at x_game = 150:\n");
    for (int y = 160; y < 283; y++) {
        Color c = GetImageColor(img, 150 + 10, y + 30);
        printf("y_game=%3d: R:%3d G:%3d B:%3d A:%3d\n", y, c.r, c.g, c.b, c.a);
    }
    
    UnloadImage(img);
    return 0;
}
