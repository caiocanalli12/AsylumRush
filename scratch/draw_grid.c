#include "raylib.h"
#include <stdio.h>

int main() {
    InitWindow(100, 100, "Grid");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    
    // Draw a 10px grid over the image in semi-transparent red,
    // and write coordinate markers every 100px.
    // Since we want to find the exact boundaries of:
    // - SCORE, TIME, RINGS yellow text
    // - White numbers 0-9
    // - Sonic life counter
    
    // Let's create an image to draw the grid on
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    
    // Let's draw horizontal lines
    for (int y = 0; y < img.height; y += 10) {
        Color gridColor = (y % 50 == 0) ? RED : (Color){255, 0, 0, 100};
        for (int x = 0; x < img.width; x++) {
            ImageDrawPixel(&img, x, y, gridColor);
        }
    }
    
    // Let's draw vertical lines
    for (int x = 0; x < img.width; x += 10) {
        Color gridColor = (x % 50 == 0) ? RED : (Color){255, 0, 0, 100};
        for (int y = 0; y < img.height; y++) {
            ImageDrawPixel(&img, x, y, gridColor);
        }
    }
    
    // Let's export this grid image so we can inspect it
    ExportImage(img, "scratch/grid_hud.png");
    printf("Exported grid image successfully.\n");
    
    UnloadImage(img);
    CloseWindow();
    return 0;
}
