#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>

typedef struct {
    Vector2 default_offset;
    Vector2 default_target;
    float   default_zoom;
} CamDefault;

void DOActionOnInput_KeyBoard();

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IMAGE.png>\n", argv[0]);
        fprintf(stderr, "Please provide an image.");
        return 1;
    }
    const int ScreenWidth = 1280;
    const int ScreenHeight = 720;
    const unsigned int MAX_TITLE_SIZE = 100;
    char *Title = (char *)malloc(sizeof(char) * MAX_TITLE_SIZE);


    int ArgLen = strlen(argv[1]);
    if (ArgLen > MAX_TITLE_SIZE) {
        fprintf(stderr, "File name is too big.\n");
        return 1;
    }
    sprintf(Title, "IMGV - %s",argv[1]);

    InitWindow(ScreenWidth, ScreenHeight, Title);

    // Setup
    SetTargetFPS(60);
    SetExitKey(KEY_Q);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    // SetConfigFlags(FLAG_FULLSCREEN_MODE);

    Font JBMono50 = LoadFontEx("./resources/JetBrainsMonoNerdFontMono-Regular.ttf", 50, NULL, 0);

    Image img = LoadImage(argv[1]);
    Texture2D Tux = LoadTextureFromImage(img);
    UnloadImage(img);

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                DrawTexture(Tux, 0, 0, WHITE);
            EndMode2D();
            DrawTextEx(JBMono50, Title, (Vector2) { .x = 10, .y = 10}, 50.0, 1, BLACK);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
