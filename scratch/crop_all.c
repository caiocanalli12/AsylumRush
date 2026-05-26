#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Crop All");
    
    // We will load the image and crop 5 regions of width 2000, height 163 (y_game = 120 to 283).
    // Let's loop and export them.
    for (int i = 0; i < 5; i++) {
        Image img = LoadImage("resources/imagens/fundo/frozensuburbs.png");
        if (img.data == NULL) {
            printf("Failed to load image at iteration %d!\n", i);
            return 1;
        }
        int start_x = i * 2000;
        Rectangle cropRec = { start_x + 10, 120 + 30, 2000, 163 };
        ImageCrop(&img, cropRec);
        char filename[100];
        sprintf(filename, "scratch/crop_region_%d.png", i);
        ExportImage(img, filename);
        UnloadImage(img);
    }
    
    CloseWindow();
    return 0;
}
