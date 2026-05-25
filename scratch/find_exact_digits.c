#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Inspect exact digits");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    
    // We scan Y=433..443, X=70..235
    // There are 10 digits: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    // They are spaced every 16 pixels:
    // 0: ~73
    // 1: ~89
    // 2: ~105
    // 3: ~121
    // 4: ~137
    // 5: ~153
    // 6: ~169
    // 7: ~185
    // 8: ~201
    // 9: ~217
    
    // Let's print out the exact non-empty boundaries of each digit in these intervals:
    for (int d = 0; d < 10; d++) {
        int guessX = 73 + d * 16;
        int startX = guessX - 4;
        int endX = guessX + 12;
        
        int minX = 999, maxX = -1;
        int minY = 999, maxY = -1;
        
        for (int x = startX; x <= endX; x++) {
            for (int y = 430; y <= 445; y++) {
                Color c = GetImageColor(img, x, y);
                // Check if color is digit (white 252,252,252 or black 0,0,0 shadow)
                bool isPart = (c.r == 252 && c.g == 252 && c.b == 252) || (c.r == 0 && c.g == 0 && c.b == 0);
                if (isPart) {
                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                }
            }
        }
        
        if (maxX != -1) {
            printf("Digit %d: X=%d, Y=%d, W=%d, H=%d (maxX=%d, maxY=%d)\n", 
                   d, minX, minY, maxX - minX + 1, maxY - minY + 1, maxX, maxY);
        } else {
            printf("Digit %d not found in interval [%d..%d]\n", d, startX, endX);
        }
    }
    
    // Also let's find the colon ':' character!
    // Let's scan in the same checkerboard region or check where it is in the sheet.
    // In Sonic, the time has colons.
    // Let's search Y=301..553, X=16..351 for any colon-like structures (usually 2 dots, width 2-4, height 6-8, separated vertically).
    // Let's write a loop to search the entire HUD box for a colon!
    printf("\nSearching for colon ':' in HUD area:\n");
    for (int x = 16; x < 351; x++) {
        for (int y = 301; y < 553; y++) {
            Color c1 = GetImageColor(img, x, y);
            // Check if we find a pixel of color white or black
            bool isPart1 = (c1.r == 252 && c1.g == 252 && c1.b == 252);
            if (isPart1) {
                // Check if this could be the top dot of a colon.
                // A colon has two dots separated by 2-4 pixels of background.
                // Let's check if there is another white pixel vertically below at y+4 or y+5 or y+6
                for (int dy = 3; dy <= 6; dy++) {
                    if (y + dy < 553) {
                        Color c2 = GetImageColor(img, x, y + dy);
                        if (c2.r == 252 && c2.g == 252 && c2.b == 252) {
                            // Found a potential colon! Let's check its width/height
                            printf("Potential colon at X=%d, Y=%d (separated by %d pixels)\n", x, y, dy);
                        }
                    }
                }
            }
        }
    }

    UnloadImage(img);
    CloseWindow();
    return 0;
}
