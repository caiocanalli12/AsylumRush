#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Inspect exact digits");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    
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
            printf("Digit %d: X=%d, Y=%d, W=%d, H=%d\n", 
                   d, minX, minY, maxX - minX + 1, maxY - minY + 1);
        } else {
            printf("Digit %d not found in interval [%d..%d]\n", d, startX, endX);
        }
    }
    
    // Let's also check for a colon character specifically in the TIME row next to digits
    // Where is the colon for TIME?
    // Let's look at the grid_hud.png. Next to TIME (which is at Y=457), there are numbers.
    // Wait, is there a colon in the HUD area at Y=430..500?
    // Let's print out potential colon at X around 200..300, Y around 450..470.
    // Actually, let's scan for a small box with height ~6, width ~2-3.
    // Let's search Y=455..465, X=100..200 for colon pixels.
    // In Sonic, the colon is drawn between hours and minutes or minutes and seconds.
    // Let's see if we can find a white colon character.
    // Let's print out all white structures in TIME row (Y=457) that are not digits.
    
    UnloadImage(img);
    CloseWindow();
    return 0;
}
