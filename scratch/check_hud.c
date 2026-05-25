#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Inspect");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    printf("Image width: %d, height: %d, format: %d\n", img.width, img.height, img.format);
    UnloadImage(img);
    CloseWindow();
    return 0;
}
