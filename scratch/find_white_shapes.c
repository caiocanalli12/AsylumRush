#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "Find Shapes");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    
    // We search the entire image for white pixels (R:252 G:252 B:252) and group them into components.
    // We print their bounding boxes.
    bool *visited = calloc(img.width * img.height, sizeof(bool));
    int count = 0;
    
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Color c = GetImageColor(img, x, y);
            if (c.r == 252 && c.g == 252 && c.b == 252 && !visited[y * img.width + x]) {
                int minX = x, maxX = x;
                int minY = y, maxY = y;
                
                // BFS
                int qCap = 10000;
                int *qX = malloc(qCap * sizeof(int));
                int *qY = malloc(qCap * sizeof(int));
                int qHead = 0, qTail = 0;
                
                qX[qTail] = x;
                qY[qTail] = y;
                qTail++;
                visited[y * img.width + x] = true;
                
                while (qHead < qTail) {
                    int cx = qX[qHead];
                    int cy = qY[qHead];
                    qHead++;
                    
                    if (cx < minX) minX = cx;
                    if (cx > maxX) maxX = cx;
                    if (cy < minY) minY = cy;
                    if (cy > maxY) maxY = cy;
                    
                    int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
                    int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};
                    
                    for (int i = 0; i < 8; i++) {
                        int nx = cx + dx[i];
                        int ny = cy + dy[i];
                        
                        if (nx >= 0 && nx < img.width && ny >= 0 && ny < img.height) {
                            Color nc = GetImageColor(img, nx, ny);
                            if (nc.r == 252 && nc.g == 252 && nc.b == 252 && !visited[ny * img.width + nx]) {
                                visited[ny * img.width + nx] = true;
                                if (qTail < qCap) {
                                    qX[qTail] = nx;
                                    qY[qTail] = ny;
                                    qTail++;
                                }
                            }
                        }
                    }
                }
                
                free(qX);
                free(qY);
                
                int w = maxX - minX + 1;
                int h = maxY - minY + 1;
                // Only print small or interesting shapes
                if (w < 15 && h < 15) {
                    printf("Shape %d: X=%d, Y=%d, W=%d, H=%d\n", count++, minX, minY, w, h);
                }
            }
        }
    }
    
    free(visited);
    UnloadImage(img);
    CloseWindow();
    return 0;
}
