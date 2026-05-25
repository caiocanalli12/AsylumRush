#include <stdio.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Extract");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    
    // We will crop several areas and export them to see if they match perfectly.
    // Coordinates are based on the grid_hud.png inspection:
    
    // 1. SCORE yellow label
    // It seems to be around X=16, Y=481, W=50, H=11
    Image scoreLabel = ImageFromImage(img, (Rectangle){16, 481, 52, 11});
    ExportImage(scoreLabel, "scratch/label_score.png");
    UnloadImage(scoreLabel);
    
    // 2. TIME yellow label
    // Around X=16, Y=505, W=38, H=11
    Image timeLabel = ImageFromImage(img, (Rectangle){16, 505, 38, 11});
    ExportImage(timeLabel, "scratch/label_time.png");
    UnloadImage(timeLabel);

    // 3. TIME red label
    // Around X=80, Y=505, W=38, H=11
    Image timeLabelRed = ImageFromImage(img, (Rectangle){80, 505, 38, 11});
    ExportImage(timeLabelRed, "scratch/label_time_red.png");
    UnloadImage(timeLabelRed);

    // 4. RINGS yellow label
    // Around X=16, Y=529, W=46, H=11
    Image ringsLabel = ImageFromImage(img, (Rectangle){16, 529, 46, 11});
    ExportImage(ringsLabel, "scratch/label_rings.png");
    UnloadImage(ringsLabel);

    // 5. RINGS red label
    // Around X=80, Y=529, W=46, H=11
    Image ringsLabelRed = ImageFromImage(img, (Rectangle){80, 529, 46, 11});
    ExportImage(ringsLabelRed, "scratch/label_rings_red.png");
    UnloadImage(ringsLabelRed);

    // 6. Life Icon / counter template:
    // Around X=16, Y=400, W=70, H=16
    Image lifeIcon = ImageFromImage(img, (Rectangle){16, 400, 70, 16});
    ExportImage(lifeIcon, "scratch/life_icon.png");
    UnloadImage(lifeIcon);

    // 7. White digits 0-9:
    // They are on the row Y=481, next to SCORE, starting around X=104.
    // Let's crop the whole row of numbers: X=104, Y=481, W=100, H=11
    Image numbersRow = ImageFromImage(img, (Rectangle){104, 481, 100, 11});
    ExportImage(numbersRow, "scratch/numbers_row.png");
    UnloadImage(numbersRow);

    UnloadImage(img);
    CloseWindow();
    printf("Successfully extracted test crops!\n");
    return 0;
}
