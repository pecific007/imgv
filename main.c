#include <raylib.h>

int main() {
    const int ScreenWidth = 1280;
    const int ScreenHeight = 720;
    const char *Title = "Hello Raylib";

    InitWindow(ScreenWidth, ScreenHeight, Title);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RED);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
