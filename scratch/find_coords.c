#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Crop");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    Color bg = GetImageColor(img, 0, 0);
    printf("Background color: R:%d G:%d B:%d A:%d\n", bg.r, bg.g, bg.b, bg.a);

    // Let's crop some rectangular regions and export them to see if we hit the numbers and texts.
    // Looking at the image, SCORE/TIME/RINGS and numbers are at the bottom left.
    // The image is 708 x 632.
    // Let's scan from y = 400 to 632, x = 0 to 400.
    // Let's write a simple visualization tool that prints bounding boxes of non-bg pixels in this area.
    
    // We can group adjacent non-bg pixels to find glyphs!
    int startY = 400;
    int endY = 632;
    int startX = 0;
    int endX = 400;

    // Let's find rows of non-bg pixels
    for (int y = startY; y < endY; y++) {
        int nonBgCount = 0;
        for (int x = startX; x < endX; x++) {
            Color c = GetImageColor(img, x, y);
            // Check if it is different from background
            if (c.r != bg.r || c.g != bg.g || c.b != bg.b) {
                nonBgCount++;
            }
        }
        if (nonBgCount > 0) {
            // Find start and end x for this row
            int firstX = -1, lastX = -1;
            for (int x = startX; x < endX; x++) {
                Color c = GetImageColor(img, x, y);
                if (c.r != bg.r || c.g != bg.g || c.b != bg.b) {
                    if (firstX == -1) firstX = x;
                    lastX = x;
                }
            }
            printf("Row %d: %d non-bg pixels, x range [%d, %d]\n", y, nonBgCount, firstX, lastX);
        }
    }

    UnloadImage(img);
    CloseWindow();
    return 0;
}
