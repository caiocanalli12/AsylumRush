#include "raylib.h"

int main() {
    InitWindow(100, 100, "Crop Digits");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    // Let's crop the entire row of digits next to SCORE
    Image digits = ImageFromImage(img, (Rectangle){100, 430, 170, 15});
    ExportImage(digits, "scratch/digits_09.png");
    UnloadImage(digits);
    CloseWindow();
    return 0;
}
