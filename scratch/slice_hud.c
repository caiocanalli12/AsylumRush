#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"

// Define a structure to keep track of a bounding box
typedef struct {
    int minX, maxX;
    int minY, maxY;
    bool active;
} Box;

int compareBoxes(const void *a, const void *b) {
    Box *boxA = (Box*)a;
    Box *boxB = (Box*)b;
    if (boxA->minY != boxB->minY) {
        return boxA->minY - boxB->minY;
    }
    return boxA->minX - boxB->minX;
}

int main() {
    InitWindow(100, 100, "Slice");
    Image img = LoadImage("resources/imagens/hud/hud.png");
    Color bg = {16, 112, 132, 255}; // True background color of the grid

    bool *visited = calloc(img.width * img.height, sizeof(bool));
    int boxCount = 0;
    Box boxes[2000] = { 0 };

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Color c = GetImageColor(img, x, y);
            bool isBg = (c.r == bg.r && c.g == bg.g && c.b == bg.b);
            
            if (!isBg && !visited[y * img.width + x]) {
                int minX = x, maxX = x;
                int minY = y, maxY = y;
                
                int qCap = 100000;
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
                            bool nIsBg = (nc.r == bg.r && nc.g == bg.g && nc.b == bg.b);
                            if (!nIsBg && !visited[ny * img.width + nx]) {
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
                
                if (boxCount < 2000) {
                    boxes[boxCount] = (Box){minX, maxX, minY, maxY, true};
                    boxCount++;
                }
            }
        }
    }

    qsort(boxes, boxCount, sizeof(Box), compareBoxes);

    printf("Total components found: %d\n", boxCount);
    // Let's filter and print components that are in the HUD area: x < 500, y > 300
    for (int i = 0; i < boxCount; i++) {
        Box b = boxes[i];
        int w = b.maxX - b.minX + 1;
        int h = b.maxY - b.minY + 1;
        if (b.minX < 450 && b.minY > 300 && w > 1 && h > 1) {
            printf("Box: x=%d, y=%d, w=%d, h=%d (range [%d..%d] x [%d..%d])\n", 
                   b.minX, b.minY, w, h, b.minX, b.maxX, b.minY, b.maxY);
        }
    }

    free(visited);
    UnloadImage(img);
    CloseWindow();
    return 0;
}
