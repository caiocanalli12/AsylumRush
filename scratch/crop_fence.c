#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Crop");
    Image img = LoadImage("resources/imagens/fundo/frozensuburbs.png");
    if (img.data == NULL) {
        printf("Failed to load image!\n");
        return 1;
    }
    
    // Crop region x = 8500 to 9500 (width 1000), y = 120 to 283 (height 163)
    // Remember, in image coordinates: x_img = x_game + 10, y_img = y_game + 30
    Rectangle cropRec = { 8500 + 10, 120 + 30, 1000, 163 };
    ImageCrop(&img, cropRec);
    
    // Save to the scratch directory
    ExportImage(img, "scratch/crop_fence.png");
    
    UnloadImage(img);
    CloseWindow();
    return 0;
}
