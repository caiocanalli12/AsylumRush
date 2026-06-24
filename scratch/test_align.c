#include "raylib.h"
#include <stdio.h>

int main() {
    InitWindow(800, 600, "Align Test");
    Texture2D tex = LoadTexture("resources/imagens/EarDog.png");
    
    RenderTexture2D target = LoadRenderTexture(800, 600);
    
    BeginTextureMode(target);
    ClearBackground(RAYWHITE);
    
    Rectangle walk[6] = {
        { 16, 29, 44, 29 },
        { 72, 29, 42, 29 },
        { 127, 29, 45, 29 },
        { 186, 29, 44, 29 },
        { 241, 29, 44, 29 },
        { 300, 29, 45, 29 },
    };
    
    Rectangle bone[5] = {
        { 379, 872, 58, 29 }, 
        { 449, 872, 58, 29 }, 
        { 520, 872, 58, 29 },
        { 591, 872, 58, 29 },     
        { 666, 872, 58, 29 },
    };
    
    for (int i = 0; i < 5; i++) {
        float x = 100 + i * 120;
        float y = 200;
        
        // Draw walk centered
        DrawTexturePro(tex, walk[i], (Rectangle){x - walk[i].width, y - walk[i].height*2, walk[i].width*2, walk[i].height*2}, (Vector2){0, 0}, 0, RED);
        
        // Draw bone centered
        DrawTexturePro(tex, bone[i], (Rectangle){x - bone[i].width, y - bone[i].height*2, bone[i].width*2, bone[i].height*2}, (Vector2){0, 0}, 0, (Color){0, 0, 255, 128});
    }
    
    EndTextureMode();
    
    Image img = LoadImageFromTexture(target.texture);
    ImageFlipVertical(&img);
    ExportImage(img, "scratch/align_all.png");
    
    CloseWindow();
    return 0;
}
