#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Inspect Numbers");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    
    // Grid background color is at (200, 200)
    Color bg = GetImageColor(img, 200, 200);
    printf("Grid background color at (200, 200): R:%d G:%d B:%d A:%d\n", bg.r, bg.g, bg.b, bg.a);

    // Let's analyze the bounding boxes of the yellow texts and numbers at specific Y ranges
    // Row of numbers next to SCORE starts at Y=481.
    // Let's print out columns of non-bg pixels at Y=481 for X in [100, 350]
    int yStart = 481;
    int yEnd = 492; // height is 11 or 12 pixels
    
    // We can find contiguous non-empty intervals of X
    bool inCol = false;
    int colStart = -1;
    int digitCount = 0;
    
    for (int x = 100; x < 350; x++) {
        bool hasNonBg = false;
        for (int y = yStart; y <= yEnd; y++) {
            Color c = GetImageColor(img, x, y);
            // Check if it is different from grid background color (bg)
            if (c.r != bg.r || c.g != bg.g || c.b != bg.b) {
                hasNonBg = true;
                break;
            }
        }
        
        if (hasNonBg && !inCol) {
            inCol = true;
            colStart = x;
        } else if (!hasNonBg && inCol) {
            inCol = false;
            int w = x - colStart;
            printf("Digit %d: X=%d, Y=%d, W=%d, H=%d\n", digitCount, colStart, yStart, w, yEnd - yStart + 1);
            digitCount++;
        }
    }
    
    // Let's do the same for yellow SCORE (around X=16 to 100, Y=481)
    printf("\nChecking SCORE label:\n");
    inCol = false;
    colStart = -1;
    for (int x = 16; x < 100; x++) {
        bool hasNonBg = false;
        for (int y = yStart; y <= yEnd; y++) {
            Color c = GetImageColor(img, x, y);
            if (c.r != bg.r || c.g != bg.g || c.b != bg.b) {
                hasNonBg = true;
                break;
            }
        }
        if (hasNonBg && !inCol) {
            inCol = true;
            colStart = x;
        } else if (!hasNonBg && inCol) {
            inCol = false;
            printf("SCORE label segment: X=%d, W=%d\n", colStart, x - colStart);
        }
    }
    
    // Checking yellow TIME (around Y=505, X=16 to 150)
    printf("\nChecking TIME label:\n");
    int yTimeStart = 505;
    int yTimeEnd = 516;
    inCol = false;
    colStart = -1;
    for (int x = 16; x < 150; x++) {
        bool hasNonBg = false;
        for (int y = yTimeStart; y <= yTimeEnd; y++) {
            Color c = GetImageColor(img, x, y);
            if (c.r != bg.r || c.g != bg.g || c.b != bg.b) {
                hasNonBg = true;
                break;
            }
        }
        if (hasNonBg && !inCol) {
            inCol = true;
            colStart = x;
        } else if (!hasNonBg && inCol) {
            inCol = false;
            printf("TIME label segment: X=%d, W=%d\n", colStart, x - colStart);
        }
    }

    // Checking yellow RINGS (around Y=529, X=16 to 150)
    printf("\nChecking RINGS label:\n");
    int yRingsStart = 529;
    int yRingsEnd = 540;
    inCol = false;
    colStart = -1;
    for (int x = 16; x < 150; x++) {
        bool hasNonBg = false;
        for (int y = yRingsStart; y <= yRingsEnd; y++) {
            Color c = GetImageColor(img, x, y);
            if (c.r != bg.r || c.g != bg.g || c.b != bg.b) {
                hasNonBg = true;
                break;
            }
        }
        if (hasNonBg && !inCol) {
            inCol = true;
            colStart = x;
        } else if (!hasNonBg && inCol) {
            inCol = false;
            printf("RINGS label segment: X=%d, W=%d\n", colStart, x - colStart);
        }
    }

    UnloadImage(img);
    CloseWindow();
    return 0;
}
