#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int minX, minY, maxX, maxY;
    int pixelCount;
} BBox;

int compareBBoxes(const void *a, const void *b) {
    BBox *ba = (BBox*)a;
    BBox *bb = (BBox*)b;
    // Sort primarily by Y, then by X
    if (abs(ba->minY - bb->minY) > 20) {
        return ba->minY - bb->minY;
    }
    return ba->minX - bb->minX;
}

int main() {
    // We don't need a visible window, but InitWindow is required for raylib image loading sometimes
    InitWindow(100, 100, "Scan");
    Image img = LoadImage("resources/imagens/EarDog.png");
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    
    int w = img.width;
    int h = img.height;
    printf("Image dimensions: %dx%d\n", w, h);
    
    Color *pixels = (Color*)img.data;
    bool *visited = calloc(w * h, sizeof(bool));
    
    BBox bboxes[1000];
    int bboxCount = 0;
    
    // Connected component analysis (BFS/DFS) to find all sprites
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = y * w + x;
            if (pixels[idx].a > 10 && !visited[idx]) {
                // Found a new sprite! Let's do BFS
                int minX = x, maxX = x;
                int minY = y, maxY = y;
                int pixelCount = 0;
                
                // BFS queue
                int queueCapacity = 10000;
                int *queueX = malloc(queueCapacity * sizeof(int));
                int *queueY = malloc(queueCapacity * sizeof(int));
                int head = 0, tail = 0;
                
                queueX[tail] = x;
                queueY[tail] = y;
                tail++;
                visited[idx] = true;
                
                while (head < tail) {
                    int cx = queueX[head];
                    int cy = queueY[head];
                    head++;
                    pixelCount++;
                    
                    if (cx < minX) minX = cx;
                    if (cx > maxX) maxX = cx;
                    if (cy < minY) minY = cy;
                    if (cy > maxY) maxY = cy;
                    
                    // 8-neighborhood
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            int nx = cx + dx;
                            int ny = cy + dy;
                            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                                int nidx = ny * w + nx;
                                if (pixels[nidx].a > 10 && !visited[nidx]) {
                                    visited[nidx] = true;
                                    if (tail >= queueCapacity) {
                                        queueCapacity *= 2;
                                        queueX = realloc(queueX, queueCapacity * sizeof(int));
                                        queueY = realloc(queueY, queueCapacity * sizeof(int));
                                    }
                                    queueX[tail] = nx;
                                    queueY[tail] = ny;
                                    tail++;
                                }
                            }
                        }
                    }
                }
                
                free(queueX);
                free(queueY);
                
                // Only keep components with a decent size (ignore noise)
                if (pixelCount > 50) {
                    bboxes[bboxCount++] = (BBox){minX, minY, maxX, maxY, pixelCount};
                }
            }
        }
    }
    
    // Sort bounding boxes so they are ordered top-to-bottom, left-to-right
    qsort(bboxes, bboxCount, sizeof(BBox), compareBBoxes);
    
    printf("Found %d sprites:\n", bboxCount);
    for (int i = 0; i < bboxCount; i++) {
        BBox b = bboxes[i];
        int bw = b.maxX - b.minX + 1;
        int bh = b.maxY - b.minY + 1;
        printf("Sprite %2d: x=%3d, y=%3d, w=%3d, h=%3d, pixels=%d\n", i, b.minX, b.minY, bw, bh, b.pixelCount);
    }
    
    // Draw bounding boxes on the image and save to inspect
    for (int i = 0; i < bboxCount; i++) {
        BBox b = bboxes[i];
        // Draw top & bottom borders
        for (int x = b.minX; x <= b.maxX; x++) {
            ImageDrawPixel(&img, x, b.minY, RED);
            ImageDrawPixel(&img, x, b.maxY, RED);
        }
        // Draw left & right borders
        for (int y = b.minY; y <= b.maxY; y++) {
            ImageDrawPixel(&img, b.minX, y, RED);
            ImageDrawPixel(&img, b.maxX, y, RED);
        }
    }
    
    ExportImage(img, "C:/Users/jdavi/.gemini/antigravity/brain/e4289673-aaea-4e49-9e3d-ae0f34d66562/eardog_detected_sprites.png");
    printf("Saved visualization to artifacts.\n");
    
    free(visited);
    UnloadImage(img);
    CloseWindow();
    return 0;
}
