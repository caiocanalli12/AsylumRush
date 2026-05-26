#include <stdio.h>
#include "raylib.h"

int main() {
    Image img = LoadImage("resources/imagens/fundo/frozensuburbs.png");
    if (img.data == NULL) {
        printf("Failed to load image!\n");
        return 1;
    }
    
    // Let's print out the column profile for x_game in [8500, 9500]
    // where the stone pillar and the diagonal fence might be.
    // The pillar in the screenshot:
    // - There is a stone pillar (dark bricks)
    // - A metal fence with spikes ("grade") going down and to the right.
    // - On the left of the pillar, the sidewalk goes further up (towards the background), which is the opening ("abertura a cima").
    // Let's print the Y coordinates of the black pixels of the fence for each X to see where it is!
    
    for (int x = 8500; x < 9500; x += 10) {
        int y_min_dark = -1;
        int y_max_dark = -1;
        int dark_count = 0;
        for (int y = 140; y < 240; y++) {
            Color c = GetImageColor(img, x + 10, y + 30);
            if (c.r < 15 && c.g < 15 && c.b < 15 && c.a > 200) {
                dark_count++;
                if (y_min_dark == -1) y_min_dark = y;
                y_max_dark = y;
            }
        }
        if (dark_count > 5) {
            printf("x=%d: dark_count=%d, y_range=[%d, %d]\n", x, dark_count, y_min_dark, y_max_dark);
        }
    }

    UnloadImage(img);
    return 0;
}
