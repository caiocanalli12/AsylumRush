#include "raylib.h"

int main() {
    InitWindow(100, 100, "Crop Colon");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    // Let's crop the potential colon at X=90, Y=226, W=5, H=9
    Image colon = ImageFromImage(img, (Rectangle){90, 226, 5, 9});
    ExportImage(colon, "scratch/colon_extracted.png");
    UnloadImage(colon);
    CloseWindow();
    return 0;
}
