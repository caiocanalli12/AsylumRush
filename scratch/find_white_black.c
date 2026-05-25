#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Digits");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    
    // Scan Y=433..444 for white digits (R:252 G:252 B:252)
    int yStart = 430;
    int yEnd = 445;
    
    printf("Scanning Y=%d..%d for R:252 G:252 B:252:\n", yStart, yEnd);
    bool inDigit = false;
    int digitStart = -1;
    int count = 0;
    
    for (int x = 100; x < 350; x++) {
        bool isDigitPixel = false;
        for (int y = yStart; y <= yEnd; y++) {
            Color c = GetImageColor(img, x, y);
            if (c.r == 252 && c.g == 252 && c.b == 252) {
                isDigitPixel = true;
                break;
            }
        }
        
        if (isDigitPixel && !inDigit) {
            inDigit = true;
            digitStart = x;
        } else if (!isDigitPixel && inDigit) {
            inDigit = false;
            // Let's find exact Y bounds for this digit
            int minY = yEnd, maxY = yStart;
            for (int dx = digitStart; dx < x; dx++) {
                for (int dy = yStart; dy <= yEnd; dy++) {
                    Color c = GetImageColor(img, dx, dy);
                    bool isPart = (c.r == 252 && c.g == 252 && c.b == 252) || (c.r == 0 && c.g == 0 && c.b == 0);
                    if (isPart) {
                        if (dy < minY) minY = dy;
                        if (dy > maxY) maxY = dy;
                    }
                }
            }
            int w = x - digitStart;
            int h = maxY - minY + 1;
            printf("Digit %d: X=%d, Y=%d, W=%d, H=%d\n", count, digitStart, minY, w, h);
            count++;
        }
    }
    
    // Also, let's find the colon character ':'
    // In Sonic, the colon is usually a couple of pixels. Let's scan from X=150 to X=250, Y=450 to Y=465
    // for small vertical structures. We'll search for white pixels.
    printf("\nScanning for colon ':' around Y=457:\n");
    inDigit = false;
    digitStart = -1;
    for (int x = 100; x < 350; x++) {
        bool isDigitPixel = false;
        for (int y = 450; y <= 465; y++) {
            Color c = GetImageColor(img, x, y);
            if (c.r == 252 && c.g == 252 && c.b == 252) {
                // Ignore if it's part of the TIME yellow/red text (which is yellow/red, not white)
                // But wait, there are white digits '0 1 2 3 4' at Y=457 too.
                // Let's see if we find any white structures that are very narrow (like width <= 3 or 4) or have gaps.
                isDigitPixel = true;
                break;
            }
        }
        if (isDigitPixel && !inDigit) {
            inDigit = true;
            digitStart = x;
        } else if (!isDigitPixel && inDigit) {
            inDigit = false;
            int minY = 465, maxY = 450;
            for (int dx = digitStart; dx < x; dx++) {
                for (int dy = 450; dy <= 465; dy++) {
                    Color c = GetImageColor(img, dx, dy);
                    bool isPart = (c.r == 252 && c.g == 252 && c.b == 252) || (c.r == 0 && c.g == 0 && c.b == 0);
                    if (isPart) {
                        if (dy < minY) minY = dy;
                        if (dy > maxY) maxY = dy;
                    }
                }
            }
            int w = x - digitStart;
            int h = maxY - minY + 1;
            printf("White item: X=%d, Y=%d, W=%d, H=%d\n", digitStart, minY, w, h);
        }
    }

    UnloadImage(img);
    CloseWindow();
    return 0;
}
