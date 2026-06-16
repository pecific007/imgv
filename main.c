#include <stdio.h>
#include <stdlib.h>
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

// States
bool StateShowHelpMenu      = true;
bool StateShowSettings      = false;
bool StateShowZoomPanel     = false;
bool StateShowMovePanel     = false;
bool StateShowMovement      = false;
bool StateProgramRunning    = true;

typedef enum {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
} Movements;

typedef enum {
    ZOOM_IN,
    ZOOM_OUT,
} ZoomType;

void Move(Camera2D *camera, Movements m) {
    const float move_factor = 10.0f / camera->zoom;
    switch (m) {
        case MOVE_UP:
            camera->target.y -= move_factor;
            break;
        case MOVE_DOWN:
            camera->target.y += move_factor;
            break;
        case MOVE_LEFT:
            camera->target.x -= move_factor;
            break;
        case MOVE_RIGHT:
            camera->target.x += move_factor;
            break;
    }
}

void Zoom(Camera2D *camera, ZoomType zt) {
    const float zoom_factor = 0.02f;
    if (zt == ZOOM_IN)
        camera->zoom = Clamp(expf(logf(camera->zoom)+zoom_factor), 0.125, 64.0);
    if (zt == ZOOM_OUT)
        camera->zoom = Clamp(expf(logf(camera->zoom)-zoom_factor), 0.125, 64.0);
}

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
    if (argv[1][ArgLen-4] != '.'
        || argv[1][ArgLen-3] != 'p'
        || argv[1][ArgLen-2] != 'n'
        || argv[1][ArgLen-1] != 'g'
    ) {
        fprintf(stderr, "File format not supported. Only `.png` images are supported at the moment.\n");
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
    UnloadTexture(Tux);
    UnloadFont(fonts.Size30);
    UnloadFont(fonts.Size50);
    UnloadFont(fonts.Size90);
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
    if (IsKeyDown(KEY_H))
        Move(camera, MOVE_LEFT);

    if (IsKeyDown(KEY_L))
        Move(camera, MOVE_RIGHT);

    // Up/down
    if (IsKeyDown(KEY_J))
        Move(camera, MOVE_DOWN);

    if (IsKeyDown(KEY_K))
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

void DrawSettingsPanel(Vector2 pos, int width, int height, LoadedFonts fonts, void* other)
{
    Vector2 BtnPos = {
        .x = width/2.0,
        .y = 50.0f,
    };
    Vector2 BtnPadding = {
        .x = width/8.0,
        .y = 5.0f,
    };
    (void)other;
    Color BtnColor  = RED;
    Font Size30     = fonts.Size30;
    const float gap = 20.0f + BtnPos.y;
    IMGV_GUI_BTN QuitBtn = CreateGUIButton("Quit", (Vector2){
        .x = BtnPos.x - (GetBtnSize("Quit", BtnPadding, Size30).x/2.0),
        .y = BtnPos.y
    }, BtnPadding, BtnColor, WHITE, Size30);
    BtnPos.y += gap;
    IMGV_GUI_BTN HelpBtn = CreateGUIButton("Help", (Vector2){
            .x = BtnPos.x - (GetBtnSize("Help", BtnPadding, Size30).x/2.0),
            .y = BtnPos.y
        }, BtnPadding, BtnColor, WHITE, Size30);
    BtnPos.y += gap;

    IMGV_GUI_BTN ShowOverlay = CreateGUIButton("Show Overlay", (Vector2){
            .x = BtnPos.x - (GetBtnSize("Show Overlay", BtnPadding, Size30).x/2.0),
            .y = BtnPos.y
        }, BtnPadding, RED, WHITE, Size30);
    if (StateShowSettings && StateShowZoomPanel) {
        ShowOverlay.BTN_Text.Text = "Hide Overlay";
    }

    DrawRectangle(pos.x, pos.y, width, height, RAYWHITE);
    if (IMGV_GUI_ButtonHover(QuitBtn)) {
        QuitBtn.BTN_Color = BLUE;
    }
    if (IMGV_GUI_ButtonPressed(QuitBtn, MOUSE_BUTTON_LEFT)) {
        StateProgramRunning = false;
    }
    DrawGUIButton(QuitBtn);
    if (IMGV_GUI_ButtonPressed(HelpBtn, MOUSE_BUTTON_LEFT)) {
        StateShowHelpMenu = true;
    }
    if (IMGV_GUI_ButtonHover(HelpBtn)) {
        HelpBtn.BTN_Color = BLUE;
    }
    DrawGUIButton(HelpBtn);
    if (IMGV_GUI_ButtonPressed(ShowOverlay, MOUSE_BUTTON_LEFT)) {
        StateShowZoomPanel = !StateShowZoomPanel;
        StateShowMovePanel = !StateShowMovePanel;
    }
    if (IMGV_GUI_ButtonHover(ShowOverlay)) {
        ShowOverlay.BTN_Color = BLUE;
    }
    DrawGUIButton(ShowOverlay);
    DrawRectangleLines(pos.x, pos.y, width, height, RED);
    return;
}

void DrawZoomPanel(Vector2 pos, int width, int height, LoadedFonts fonts, void* other) {
    DrawRectangle(pos.x, pos.y, width, height, RED);
    IMGV_GUI_BTN ZoomIN = CreateGUIButton("+", (Vector2) {
        .x = pos.x, .y = pos.y,
    }, (Vector2) { .x = width/6.0, .y = 5}, WHITE, RED, fonts.Size30);
    IMGV_GUI_BTN ZoomOUT = CreateGUIButton("-", (Vector2) {
        .x = pos.x+width/2.0, .y = pos.y,
    }, (Vector2) { .x = width/6.0, .y = 5}, WHITE, RED, fonts.Size30);
    if (IMGV_GUI_ButtonHover(ZoomIN)) {
        ZoomIN.BTN_Color = BLUE;
        ZoomIN.BTN_Text.Color = WHITE;
    }
    if (IMGV_GUI_ButtonDown(ZoomIN, MOUSE_BUTTON_LEFT)) {
        Zoom(other, ZOOM_IN); // Other is camera in this case
    }
    DrawGUIButton(ZoomIN);
    if (IMGV_GUI_ButtonHover(ZoomOUT)) {
        ZoomOUT.BTN_Color = BLUE;
        ZoomOUT.BTN_Text.Color = WHITE;
    }
    if (IMGV_GUI_ButtonDown(ZoomOUT, MOUSE_BUTTON_LEFT)) {
        Zoom(other, ZOOM_OUT);
    }
    DrawGUIButton(ZoomOUT);
    DrawRectangleLines(pos.x, pos.y, width, height, RED);
    return;
}

#define BTNSIZE(ltr) GetBtnSize(ltr, BtnPadding, fonts.Size30)
void DrawMovePanel(Vector2 pos, int width, int height, LoadedFonts fonts, void* other)
{
    Vector2 BtnPadding = { .x = 15, .y = 10 };
    Vector2 BtnPos     = { .x = pos.x, .y = pos.y };
    const float PadBg = 5.0f;
    IMGV_GUI_BTN Mv_u = CreateGUIButton("U", (Vector2) {
        .x = BtnPos.x + width/2.0 - BTNSIZE("U").x/2.0 ,
        .y = BtnPos.y + PadBg
    }, BtnPadding, WHITE, RED, fonts.Size30);

    IMGV_GUI_BTN Mv_l = CreateGUIButton("L", (Vector2) {
        .x = BtnPos.x + PadBg,
        .y = BtnPos.y += height/2.0 - BTNSIZE("L").y/2.0
    }, BtnPadding, WHITE, RED, fonts.Size30);

    IMGV_GUI_BTN Mv_r = CreateGUIButton("R", (Vector2) {
        .x = BtnPos.x + (width/2.0 - BTNSIZE("R").x/2.0) *2 - PadBg,
        .y = BtnPos.y
    }, BtnPadding, WHITE, RED, fonts.Size30);

    IMGV_GUI_BTN Mv_d = CreateGUIButton("D", (Vector2) {
        .x = BtnPos.x + width/2.0 - BTNSIZE("D").x/2.0,
        .y = pos.y + height - BTNSIZE("D").y - PadBg
    }, BtnPadding, WHITE, RED, fonts.Size30);

    // DrawRectangle(pos.x, pos.y,  width,  height, RED);
    float rad = width/2.0;
    DrawCircle(rad + pos.x, rad + pos.y, rad, WHITE);
    if (IMGV_GUI_ButtonHover(Mv_u)) {
        Mv_u.BTN_Color = BLUE;
        Mv_u.BTN_Text.Color = WHITE;
    }
    if (IMGV_GUI_ButtonDown(Mv_u, MOUSE_BUTTON_LEFT)) {
        Move(other, MOVE_UP);
    }
    DrawGUIButton(Mv_u);
    if (IMGV_GUI_ButtonHover(Mv_l)) {
        Mv_l.BTN_Color = BLUE;
        Mv_l.BTN_Text.Color = WHITE;
    }
    if (IMGV_GUI_ButtonDown(Mv_l, MOUSE_BUTTON_LEFT)) {
        Move(other, MOVE_LEFT);
    }
    DrawGUIButton(Mv_l);
    if (IMGV_GUI_ButtonHover(Mv_r)) {
        Mv_r.BTN_Color = BLUE;
        Mv_r.BTN_Text.Color = WHITE;
    }
    if (IMGV_GUI_ButtonDown(Mv_r, MOUSE_BUTTON_LEFT)) {
        Move(other, MOVE_RIGHT);
    }
    DrawGUIButton(Mv_r);
    if (IMGV_GUI_ButtonHover(Mv_d)) {
        Mv_d.BTN_Color = BLUE;
        Mv_d.BTN_Text.Color = WHITE;
    }
    if (IMGV_GUI_ButtonDown(Mv_d, MOUSE_BUTTON_LEFT)) {
        Move(other, MOVE_DOWN);
    }
    DrawGUIButton(Mv_d);
    // DrawRectangleLines(pos.x, pos.y, width, height, RED);
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
