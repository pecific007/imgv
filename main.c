#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

#define IMGV_GUI_IMPLEMENTATION
#include "./IMGV_GUI.h"

typedef struct {
    Font Size30;
    Font Size50;
    Font Size90;
} LoadedFonts;

void CenterCameraOnTux(Camera2D *camera, Texture2D Tux);
void DoActionOnInput_KeyBoard(Camera2D *camera, Texture2D Tux);
void DoActionOnInput_Mouse(Camera2D *camera);
void ShowHelpGui(LoadedFonts fonts);

bool StateShowHelpMenu = true;
bool StateProgramRunning    = true;

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
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE); Doesn't work well with tilingWM

    InitWindow(ScreenWidth, ScreenHeight, Title);
        SetTargetFPS(60);
        SetExitKey(KEY_Q);

        char *FontFilePath = "./resources/JetBrainsMonoNerdFontMono-Regular.ttf";
        LoadedFonts fonts = {
            .Size30 = LoadFontEx(FontFilePath, 30.0f, NULL, 0),
            .Size50 = LoadFontEx(FontFilePath, 50.0f, NULL, 0),
            .Size90 = LoadFontEx(FontFilePath, 90.0f, NULL, 0),
        };

        Image img = LoadImage(argv[1]);
        Texture2D Tux = LoadTextureFromImage(img);
        UnloadImage(img);
        Camera2D camera = { 0 };
        camera.zoom = 1.0f;
        CenterCameraOnTux(&camera, Tux);

        while (!WindowShouldClose()) {
            BeginDrawing();
                DoActionOnInput_KeyBoard(&camera, Tux);
                DoActionOnInput_Mouse(&camera);
                ClearBackground(BLACK);
                if (!StateProgramRunning) {
                    EndDrawing();
                    goto close_prog;
                }
                if (IsKeyPressed(KEY_F1))
                    StateShowHelpMenu = !StateShowHelpMenu;
                if (StateShowHelpMenu) {
                    ShowHelpGui(fonts);
                } else {
                    BeginMode2D(camera);
                        DrawTexture(Tux, 0, 0, WHITE);
                    EndMode2D();
                }
            EndDrawing();
        }

close_prog:
    UnloadTexture(Tux);
    UnloadFont(fonts.Size30);
    UnloadFont(fonts.Size50);
    UnloadFont(fonts.Size90);
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

void ShowHelpGui(LoadedFonts fonts)
{
    char *title   = "IMGV - HELP MENU\0";
    Vector2 Position = {
        .x = (GetScreenWidth() - MeasureTextEx(fonts.Size90, title, fonts.Size90.baseSize, 1).x) /2.0,
        .y = GetScreenHeight()/2.0 - 350.0
    };

    char *text[] = {
        "F1  : Open/Close this help menu",
        "0   : Reset zoom/movement",
        "l   : Move image right",
        "h   : Move image left",
        "j   : Move image down",
        "k   : Move image up",
        "c   : Center image",
        "=/i : Zoom in",
        "-/u : Zoom out",
        "q   : Quit",
    };
    int len = (sizeof (text) / sizeof(text[0]));
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), RAYWHITE);
    DrawTextEx(fonts.Size90, title, Position, fonts.Size90.baseSize, 1, BLACK);
    DrawTextEx(fonts.Size50, "Keybinds:", (Vector2) {
        .x = Position.x += MeasureTextEx(fonts.Size90, title, fonts.Size90.baseSize, 1).x /4.0,
        .y = Position.y += 100,
    }, fonts.Size50.baseSize, 1, BLACK);
    DrawLine(0, Position.y - 5, GetScreenWidth(), Position.y - 5, BLUE);
    Position.y += 20;
    for (int i = 0; i < len; ++i) {
        DrawTextEx(fonts.Size30, text[i], (Vector2) {
            .x = Position.x,
            .y = Position.y += 45,
        }, fonts.Size30.baseSize, 1, BLACK);
    }
    Vector2 BtnPadding = {
        .x = 50,
        .y = 10,
    };
    float BtnBottomY = BUTTON_BOTTOM_Y(BtnPadding.y, 10);
    Color BtnColor = RED;
    Color BtnTextColor = WHITE;

    IMGV_GUI_BTN QuitBtn = CreateGUIButton("Quit",
        (Vector2) { .x = 10, .y = BtnBottomY },
        BtnPadding, BtnColor, BtnTextColor, fonts.Size30, BTN_QUIT);
    IMGV_GUI_BTN ContinueBtn =  CreateGUIButton("Continue",
        (Vector2) { .x = BUTTON_RIGHT_X(BtnPadding.x, 10, "Continue", fonts.Size30), .y = BtnBottomY },
        BtnPadding, BtnColor, BtnTextColor, fonts.Size30, BTN_CONT);
    if (IMGV_GUI_ButtonHover(QuitBtn)) {
        QuitBtn.BTN_Color = BLUE;
    }
    if (IMGV_GUI_ButtonPressed(QuitBtn, MOUSE_BUTTON_LEFT)) {
        StateProgramRunning = false;
    }
    DrawGUIButton(QuitBtn);
    if (IMGV_GUI_ButtonHover(ContinueBtn)) {
        ContinueBtn.BTN_Color = BLUE;
    }
    if (IMGV_GUI_ButtonPressed(ContinueBtn, MOUSE_BUTTON_LEFT)) {
        StateShowHelpMenu = false;
    }
    DrawGUIButton(ContinueBtn);
    return;
}

void DoActionOnInput_Mouse(Camera2D *camera)
{
    // Move img
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0/camera->zoom);
        camera->target = Vector2Add(camera->target, delta);
        return;
    }

    // Zoom in/out
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        Vector2 MousePos = GetMousePosition();
        Vector2 MouseWorldPos = GetScreenToWorld2D(MousePos, *camera);
        camera->offset = MousePos;
        camera->target = MouseWorldPos;
        camera->zoom = Clamp(expf(logf(camera->zoom)+0.1*wheel), 0.125, 64.0);
    }
}

void DoActionOnInput_KeyBoard(Camera2D *camera, Texture2D Tux)
{
    const float zoom_factor = 0.02f;
    const float move_factor = 10.0f / camera->zoom;

    // Resets
    if (IsKeyDown(KEY_ZERO)) {
        camera->zoom = 1.0f;
        CenterCameraOnTux(camera, Tux);
    }
    if (IsKeyDown(KEY_C))
        CenterCameraOnTux(camera, Tux);

    // Left/right
    if (IsKeyDown(KEY_H))
        camera->target.x -= move_factor;

    if (IsKeyDown(KEY_L))
        camera->target.x += move_factor;

    // Up/down
    if (IsKeyDown(KEY_J))
        camera->target.y += move_factor;

    if (IsKeyDown(KEY_K))
        camera->target.y -= move_factor;


    // Zooming
    if (IsKeyDown(KEY_U) || IsKeyDown(KEY_MINUS))
        camera->zoom = Clamp(expf(logf(camera->zoom)-zoom_factor), 0.125, 64.0);
    if (IsKeyDown(KEY_I) || IsKeyDown(KEY_EQUAL))
        camera->zoom = Clamp(expf(logf(camera->zoom)+zoom_factor), 0.125, 64.0);
}
