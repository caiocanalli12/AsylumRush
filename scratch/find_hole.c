#include <stdio.h>
#include "raylib.h"

int main() {
    Image img = LoadImage("resources/imagens/fundo/frozensuburbs.png");
    if (img.data == NULL) {
        printf("Failed to load image!\n");
        return 1;
    }
    
    int start_hole = -1;
    for (int x = 0; x < 10603; x++) {
        Color c = GetImageColor(img, x + 10, 220 + 30);
        int is_sidewalk = (c.a > 0) && (c.r > 15 || c.b > 50) && !(c.g > 30 && c.g > c.r && c.g > c.b);
        if (!is_sidewalk) {
            if (start_hole == -1) {
                start_hole = x;
            }
        } else {
            if (start_hole != -1) {
                int end_hole = x - 1;
                int width = end_hole - start_hole + 1;
                if (width > 15) {
                    printf("Sidewalk gap: x = %d to %d (width = %d)\n", start_hole, end_hole, width);
                }
                start_hole = -1;
            }
        }
    }
    if (start_hole != -1) {
        int end_hole = 10602;
        int width = end_hole - start_hole + 1;
        if (width > 15) {
            printf("Sidewalk gap: x = %d to %d (width = %d)\n", start_hole, end_hole, width);
        }
    }
    
    UnloadImage(img);
    return 0;
}
