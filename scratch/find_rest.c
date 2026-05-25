#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Inspect Rest");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    
    // 1. Scan for digits 0-9 up to X=300, Y=430..445
    printf("Scanning Y=430..445 for digits 0-9:\n");
    bool inDigit = false;
    int digitStart = -1;
    int count = 0;
    for (int x = 100; x < 300; x++) {
        bool isDigitPixel = false;
        for (int y = 430; y <= 445; y++) {
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
            int minY = 445, maxY = 430;
            for (int dx = digitStart; dx < x; dx++) {
                for (int dy = 430; dy <= 445; dy++) {
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
    
    // 2. Scan around Y=340..370, X=100..130 for the colon ':'
    printf("\nScanning Y=340..370, X=100..130 for colon:\n");
    inDigit = false;
    digitStart = -1;
    for (int x = 100; x < 130; x++) {
        bool isDigitPixel = false;
        for (int y = 340; y <= 370; y++) {
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
            int minY = 370, maxY = 340;
            for (int dx = digitStart; dx < x; dx++) {
                for (int dy = 340; dy <= 370; dy++) {
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
            printf("Colon/Item: X=%d, Y=%d, W=%d, H=%d\n", digitStart, minY, w, h);
        }
    }

    UnloadImage(img);
    CloseWindow();
    return 0;
}
