#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

#define IMGV_GUI_IMPLEMENTATION
#include "./includes/imgv_gui.h"

void CenterCameraOnTux(Camera2D *camera, Texture2D Tux);
void DoActionOnInput_KeyBoard(Camera2D *camera, Texture2D Tux);
void DoActionOnInput_Mouse(Camera2D *camera);
void ShowHelpGui(LoadedFonts fonts);
void DrawOverlayGUI(LoadedFonts fonts, Camera2D *camera);

typedef enum {
    IMGV_INI,
    IMGV_CMD,
    IMGV_GUI,
} IMGV_MODES;

void IMGV_LoadTux(Image *img, Texture2D *Tux, char *Path) {
    *img = LoadImage(Path);
    *Tux = LoadTextureFromImage(*img);
}

bool DrawDropOffScreen(Camera2D *camera, Image *img, Texture2D *Tux, FilePathList *fpl, Font font)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), RAYWHITE);
    DrawTextEx(font, "Drop Image", (Vector2){
            GetScreenWidth()/2.0f -
            MeasureTextEx(font, "Drop Image",
                font.baseSize, 1).x/2.0f,
            GetScreenHeight()/2.0f - font.baseSize
        },
        font.baseSize, 1, LIGHTGRAY);
    if (IsFileDropped()) {
        *fpl = LoadDroppedFiles();
        if (fpl->count > 0) {
            IMGV_LoadTux(img, Tux, fpl->paths[0]);
            StateImageLoaded = true;
            CenterCameraOnTux(camera, *Tux);
            return true;
        }
    }
    return false;
}


int main(int argc, char **argv)
{
    IMGV_MODES mode = IMGV_INI;
    if (argc > 1) mode = IMGV_CMD; else mode = IMGV_GUI;
    // mode = IMGV_CMD;

    if (mode == IMGV_CMD) {
        if (argc != 3 || argc == 2) {
            fprintf(stderr, "Usage: %s -i <IMAGE.png>\n", argv[0]);
            fprintf(stderr, "Please provide an image.\n");
            return 1;
        }
        if (argc == 3 && strcmp(argv[1], "-i") != 0) {
            fprintf(stderr, "Invalid flag.\n");
            return 1;
        }
        StateShowDropoff = false;
    }

    const int ScreenWidth = 1280;
    const int ScreenHeight = 720;
    const unsigned int MAX_TITLE_SIZE = 100;
    if (strlen(argv[2]) > MAX_TITLE_SIZE) {
        fprintf(stderr, "File name is too big.\n");
        return 1;
    }
    char *Title = "IMGV - Image Viewer";

    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Doesn't work well with tilingWM

    InitWindow(ScreenWidth, ScreenHeight, Title);
        SetTargetFPS(60);
        SetExitKey(KEY_Q);

        char *FontFilePath = "./resources/JetBrainsMonoNerdFontMono-Regular.ttf";
        LoadedFonts fonts = {
            .Size30 = LoadFontEx(FontFilePath, 30.0f, NULL, 0),
            .Size50 = LoadFontEx(FontFilePath, 50.0f, NULL, 0),
            .Size90 = LoadFontEx(FontFilePath, 90.0f, NULL, 0),
        };

        FilePathList fpl = {0};
        Image img = {0};
        Texture2D Tux = {0};
        Camera2D camera = { 0 };
        camera.zoom = 1.0f;
        while (!WindowShouldClose()) {
            BeginDrawing();
                if (!StateImageLoaded && mode == IMGV_GUI) {
                    if (DrawDropOffScreen(&camera, &img, &Tux, &fpl, fonts.Size90)) {
                        StateImageLoaded = true;
                    }
                } else if (!StateImageLoaded && mode == IMGV_CMD) {
                    IMGV_LoadTux(&img, &Tux, argv[2]);
                    StateImageLoaded = true;
                    CenterCameraOnTux(&camera, Tux);
                }
                ClearBackground(BLACK);
                if (!StateProgramRunning)
                    goto close_prog;
                if (IsKeyPressed(KEY_F1))
                    StateShowHelpMenu = !StateShowHelpMenu;
                if (StateShowHelpMenu) {
                    ShowHelpGui(fonts);
                } else {
                    DoActionOnInput_KeyBoard(&camera, Tux);
                    DoActionOnInput_Mouse(&camera);
                    BeginMode2D(camera);
                        DrawTexture(Tux, 0, 0, WHITE);
                    EndMode2D();
                    DrawOverlayGUI(fonts, &camera);
                }
            EndDrawing();
        }

close_prog:
    UnloadFont(fonts.Size30);
    UnloadFont(fonts.Size50);
    UnloadFont(fonts.Size90);
    UnloadTexture(Tux);
    UnloadImage(img);
    CloseWindow();
    return 0;
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

void DoActionOnInput_KeyBoard(Camera2D *camera, Texture2D Tux)
{
    // Resets
    if (IsKeyDown(KEY_ZERO)) {
        camera->zoom = 1.0f;
        CenterCameraOnTux(camera, Tux);
    }
    if (IsKeyDown(KEY_C))
        CenterCameraOnTux(camera, Tux);

    // Expand/Collapse pannel
    // Settings
    if (IsKeyPressed(KEY_S))
        StateShowSettings = !StateShowSettings;

    // Zoom
    if (IsKeyPressed(KEY_Z))
        StateShowZoomPanel = !StateShowZoomPanel;

    // Move
    if (IsKeyPressed(KEY_M))
        StateShowMovePanel = !StateShowMovePanel;

    // Left/right
    if (IsKeyDown(KEY_H) || IsKeyDown(KEY_LEFT))
        Move(camera, MOVE_LEFT);

    if (IsKeyDown(KEY_L) || IsKeyDown(KEY_RIGHT))
        Move(camera, MOVE_RIGHT);

    // Up/down
    if (IsKeyDown(KEY_J) || IsKeyDown(KEY_DOWN))
        Move(camera, MOVE_DOWN);

    if (IsKeyDown(KEY_K) || IsKeyDown(KEY_UP))
        Move(camera, MOVE_UP);

    // Zooming
    if (IsKeyDown(KEY_I) || IsKeyDown(KEY_MINUS))
        Zoom(camera, ZOOM_OUT);
    if (IsKeyDown(KEY_U) || IsKeyDown(KEY_EQUAL))
        Zoom(camera, ZOOM_IN);
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
        "s   : Show/Hide settings panel",
        "0   : Reset zoom/movement",
        "l   : Move image right",
        "h   : Move image left",
        "j   : Move image down",
        "k   : Move image up",
        "c   : Center image",
        "z   : Show zoom panel",
        "m   : Show move panel",
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
            .y = Position.y += 35,
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
        BtnPadding, BtnColor, BtnTextColor, fonts.Size30);
    IMGV_GUI_BTN ContinueBtn =  CreateGUIButton("Continue",
        (Vector2) { .x = BUTTON_RIGHT_X(BtnPadding.x, 10, "Continue", fonts.Size30), .y = BtnBottomY },
        BtnPadding, BtnColor, BtnTextColor, fonts.Size30);
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


void DrawOverlayGUI(LoadedFonts fonts, Camera2D *camera)
{
    // Drawing Settings panel
    PanelData Settings = { .DrawFn = DrawSettingsPanel, .OtherData = NULL };
    DrawPanelWidgets("Settings", (Vector2) { 0 }, WIDGET_LEFT,
        GetScreenWidth()/4, GetScreenHeight(), &StateShowSettings, fonts, Settings);

    // Drawing Zoom panel
    PanelData ZoomPanel = { .DrawFn = DrawZoomPanel, .OtherData = camera };
    DrawPanelWidgets("Zoom", (Vector2) { .x = 1200, .y = 0 }, WIDGET_RIGHT,
        80, 40, &StateShowZoomPanel, fonts, ZoomPanel);

    // Drawing Zoom panel
    PanelData MovePanel = { .DrawFn = DrawMovePanel, .OtherData = camera };
    DrawPanelWidgets("Move", (Vector2) { .x = 1200, .y = 500 }, WIDGET_RIGHT,
        180, 180, &StateShowMovePanel, fonts, MovePanel);
    return;
}
