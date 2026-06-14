#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

typedef struct {
    Vector2 default_offset;
    Vector2 default_target;
    float   default_zoom;
} CamDefault;

void CenterCameraOnTux(Camera2D *camera, Texture2D Tux);
void DoActionOnInput_KeyBoard(Camera2D *camera, Texture2D Tux);
void DoActionOnInput_Mouse(Camera2D *camera);

CamDefault CameraDefault = {
    .default_offset = { 0 },
    .default_target = { 0 },
    .default_zoom   = 1.0f,
};
int CenteredTuxOnStartup = 0;


int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IMAGE.png>\n", argv[0]);
        fprintf(stderr, "Please provide an image.\n");
        return 1;
    }
    const int ScreenWidth = 1280;
    const int ScreenHeight = 720;
    const unsigned int MAX_TITLE_SIZE = 100;
    char *Title = (char *)malloc(sizeof(char) * MAX_TITLE_SIZE);


    unsigned int ArgLen = strlen(argv[1]);
    if (ArgLen > MAX_TITLE_SIZE) {
        fprintf(stderr, "File name is too big.\n");
        return 1;
    }
    sprintf(Title, "IMGV - %s",argv[1]);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(ScreenWidth, ScreenHeight, Title);
        SetTargetFPS(60);
        SetExitKey(KEY_Q);

        Font JBMono50 = LoadFontEx("./resources/JetBrainsMonoNerdFontMono-Regular.ttf", 50, NULL, 0);

        Image img = LoadImage(argv[1]);
        Texture2D Tux = LoadTextureFromImage(img);
        UnloadImage(img);
        Camera2D camera = { 0 };
        camera.zoom = 1.0f;

        while (!WindowShouldClose()) {
            BeginDrawing();
                DoActionOnInput_KeyBoard(&camera, Tux);
                DoActionOnInput_Mouse(&camera);
                ClearBackground(BLACK);
                BeginMode2D(camera);
                    DrawTexture(Tux, 0, 0, WHITE);
                EndMode2D();
                DrawTextEx(JBMono50, Title, (Vector2) { .x = 10, .y = 10}, 50.0, 1, WHITE);
            EndDrawing();
        }
    UnloadTexture(Tux);
    UnloadFont(JBMono50);
    CloseWindow();
    return 0;
}

void CenterCameraOnTux(Camera2D *camera, Texture2D Tux)
{
    camera->offset = (Vector2) {
        .x = GetScreenWidth() /2.0f,
        .y = GetScreenHeight()/2.0f,
    };
    camera->target = (Vector2) {
        .x = Tux.width /2.0f,
        .y = Tux.height/2.0f,
    };
}

void DoActionOnInput_Mouse(Camera2D *camera)
{
    // ---------- MOUSE ACTINOS ---------- //
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0/camera->zoom);
        camera->target = Vector2Add(camera->target, delta);
        return;
    }
    float WheelMovement = GetMouseWheelMove();
    float zoom_factor = 0.1*WheelMovement;
    if (WheelMovement != 0) {
        Vector2 MousePos = GetMousePosition();
        Vector2 MouseWorldPos = GetScreenToWorld2D(MousePos, *camera);
        camera->offset = MousePos;
        camera->target = MouseWorldPos;
        camera->zoom = Clamp(expf(logf(camera->zoom)+zoom_factor), 0.125, 64.0);
        return;
    }
}

void DoActionOnInput_KeyBoard(Camera2D *camera, Texture2D Tux)
{
    // ---------- KEYBOARD ACTINOS ---------- //
    const float zoom_factor = 0.02f;
    const float move_factor = 10.0f;
    if (CenteredTuxOnStartup == 2) {
        CenterCameraOnTux(camera, Tux);
        CenteredTuxOnStartup = 5;
    } else if (CenteredTuxOnStartup < 5) {
        CenteredTuxOnStartup++;
    }

    if (IsKeyDown(KEY_ZERO)) {
        camera->zoom = 1.0f;
        CenterCameraOnTux(camera, Tux);
    }
    if (IsKeyDown(KEY_C))
        CenterCameraOnTux(camera, Tux);
    if (IsKeyDown(KEY_H))
        camera->offset.x -= move_factor;
    if (IsKeyDown(KEY_J))
        camera->offset.y += move_factor;
    if (IsKeyDown(KEY_K))
        camera->offset.y -= move_factor;
    if (IsKeyDown(KEY_L))
        camera->offset.x += move_factor;
    if (IsKeyDown(KEY_U) || IsKeyDown(KEY_MINUS))
        camera->zoom = Clamp(expf(logf(camera->zoom)-zoom_factor), 0.125, 64.0);
    if (IsKeyDown(KEY_I) || IsKeyDown(KEY_EQUAL))
        camera->zoom = Clamp(expf(logf(camera->zoom)+zoom_factor), 0.125, 64.0);
}
