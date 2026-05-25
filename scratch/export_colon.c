#include "raylib.h"

int main() {
    InitWindow(100, 100, "Crop Colon");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    Image colon = ImageFromImage(img, (Rectangle){258, 456, 6, 11});
    ExportImage(colon, "scratch/colon.png");
    UnloadImage(colon);
    CloseWindow();
    return 0;
}
