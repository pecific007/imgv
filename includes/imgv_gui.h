#pragma once

#ifndef IMGV_GUI_H
#define IMGV_GUI_H

#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#define BUTTON_BOTTOM_Y(BtnPaddingY, PaddingY) \
    GetScreenHeight() - (BtnPaddingY*2 + fonts.Size30.baseSize + PaddingY)
#define BUTTON_RIGHT_X(BtnPaddingX, PaddingX, text, font) \
    GetScreenWidth() - (BtnPaddingX*2 + (MeasureTextEx(font, text, font.baseSize, 1).x) + PaddingX)

typedef struct {
    Font          Font;
    Vector2       Pos;
    Color         Color;
    const char   *Text;
} IMGV_BTN_TEXT;

typedef struct {
    Vector2       BTN_Pos;
    Vector2       BTN_Size;
    Vector2       BTN_Padding;
    Color         BTN_Color;
    IMGV_BTN_TEXT BTN_Text;
} IMGV_GUI_BTN;

typedef struct {
    Font Size30;
    Font Size50;
    Font Size90;
} LoadedFonts;

typedef enum {
    WIDGET_LEFT,
    WIDGET_RIGHT,
} WIDGET_Orientation;

typedef struct {
    void    (*DrawFn) (Vector2, int, int, LoadedFonts, void*);
    void    *OtherData;
} PanelData;

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


// States
bool StateShowDropoff       = true;
bool StateImageLoaded       = false;
bool StateShowHelpMenu      = true;
bool StateShowSettings      = false;
bool StateShowZoomPanel     = false;
bool StateShowMovePanel     = false;
bool StateShowMovement      = false;
bool StateProgramRunning    = true;

void Zoom(Camera2D *camera, ZoomType zt);
void Move(Camera2D *camera, Movements m);
IMGV_GUI_BTN CreateGUIButton(const char *text,
    Vector2 pos, Vector2 padding, Color BtnColor,
    Color TextColor, Font font);
void DrawGUIButton(IMGV_GUI_BTN Button);
bool IMGVGUIButtonPressed(IMGV_GUI_BTN Button);
bool IMGV_GUI_ButtonHover(IMGV_GUI_BTN Button);
bool IMGV_GUI_ButtonPressed(IMGV_GUI_BTN Button, MouseButton click);
bool IMGV_GUI_ButtonDown(IMGV_GUI_BTN Button, MouseButton click);
Vector2 GetBtnSize(const char *text, Vector2 padding, Font font);
void DrawPanelWidgets(const char *PanelName, Vector2 WidgetPos, WIDGET_Orientation ori,
    int width, int height, bool *State, LoadedFonts font, PanelData Pdata);

#endif // IMGV_GUI_H

// #define IMGV_GUI_IMPLEMENTATION // For debugging/developing

#ifdef IMGV_GUI_IMPLEMENTATION

void Move(Camera2D *camera, Movements m)
{
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

void Zoom(Camera2D *camera, ZoomType zt)
{
    const float zoom_factor = 0.02f;
    if (zt == ZOOM_IN)
        camera->zoom = Clamp(expf(logf(camera->zoom)+zoom_factor), 0.125, 64.0);
    if (zt == ZOOM_OUT)
        camera->zoom = Clamp(expf(logf(camera->zoom)-zoom_factor), 0.125, 64.0);
}

// ------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------ DRAWING UI ------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------------ //

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

// ----------------------------------------------------------------------------------------------------------------------------------- //
// ----------------------------------------------------------------------------------------------------------------------------------- //
// ------------------------------------------------------ IMPLEMENTATION OF GUI ------------------------------------------------------ //
// ------------------------------------------------------   (internal  logic)   ------------------------------------------------------ //
// ----------------------------------------------------------------------------------------------------------------------------------- //
// ----------------------------------------------------------------------------------------------------------------------------------- //

IMGV_GUI_BTN CreateGUIButton(const char *text,
    Vector2 pos, Vector2 padding, Color BtnColor,
    Color TextColor, Font font)
{
    Vector2 TextPos = {
        .x = padding.x + pos.x,
        .y = padding.y + pos.y,
    };
    Vector2 BtnSize = {
        .x = (padding.x + MeasureTextEx(font, text, font.baseSize, 1).x + padding.x),
        .y = (padding.y + font.baseSize + padding.y),
    };
    IMGV_BTN_TEXT BtnText = {
        .Font = font,
        .Pos  = TextPos,
        .Color = TextColor,
        .Text = text,
    };
    return (IMGV_GUI_BTN) {
        .BTN_Color = BtnColor,
        .BTN_Padding = padding,
        .BTN_Size = BtnSize,
        .BTN_Text = BtnText,
        .BTN_Pos  = pos,
    };
}

void DrawGUIButton(IMGV_GUI_BTN Button)
{
    DrawRectangleV(Button.BTN_Pos, Button.BTN_Size, Button.BTN_Color);
    DrawTextEx(Button.BTN_Text.Font,
        Button.BTN_Text.Text,
        Button.BTN_Text.Pos,
        Button.BTN_Text.Font.baseSize,
        1, Button.BTN_Text.Color);
    return;
}

bool IMGV_GUI_ButtonHover(IMGV_GUI_BTN Button)
{
    Vector2 MousePos = GetMousePosition();
    if (MousePos.y >= Button.BTN_Pos.y
        && MousePos.y <= (Button.BTN_Pos.y + Button.BTN_Size.y)
        && MousePos.x >= Button.BTN_Pos.x
        && MousePos.x <= (Button.BTN_Pos.x + Button.BTN_Size.x)
    ) {
        return true;
    }
    return false;
}

bool IMGV_GUI_ButtonPressed(IMGV_GUI_BTN Button, MouseButton click)
{
    if (!IMGV_GUI_ButtonHover(Button)) return false;
    if (IsMouseButtonPressed(click)) {
        return true;
    }
    return false;
}

bool IMGV_GUI_ButtonDown(IMGV_GUI_BTN Button, MouseButton click)
{
    if (!IMGV_GUI_ButtonHover(Button)) return false;
    if (IsMouseButtonDown(click)) {
        return true;
    }
    return false;
}

inline Vector2 GetBtnSize(const char *text, Vector2 padding, Font font)
{
    return (Vector2) {
        .x = (padding.x + MeasureTextEx(font, text, font.baseSize, 1).x + padding.x),
        .y = (padding.y + font.baseSize + padding.y),
    };
}

// Passing functions as parameters from:
// https://stackoverflow.com/questions/9410/how-do-you-pass-a-function-as-a-parameter-in-c
void DrawPanelWidgets(const char *PanelName, Vector2 WidgetPos, WIDGET_Orientation ori,
    int width, int height, bool *State, LoadedFonts font, PanelData Pdata)
{
    Vector2 WidgetPadding = {
        .x = 5.0f,
        .y = 5.0f,
    };
    Vector2 ToolTipPadding = {
        .x = 15.0f,
        .y = 5.0f,
    };
    IMGV_GUI_BTN WidgetBtn = CreateGUIButton(">", WidgetPos, WidgetPadding, WHITE, RED, font.Size30);
    IMGV_GUI_BTN ToolTip = CreateGUIButton(TextFormat("Expand %s", PanelName),
        (Vector2) {
            .x = WidgetBtn.BTN_Pos.x + WidgetBtn.BTN_Size.x + 10,
            .y = WidgetBtn.BTN_Pos.y,
        }, ToolTipPadding, WHITE, RED, font.Size30);

    if (ori == WIDGET_RIGHT) {
        WidgetBtn.BTN_Text.Text = "<";
        WidgetBtn.BTN_Pos.x = GetScreenWidth() - WidgetBtn.BTN_Size.x;
        WidgetBtn.BTN_Text.Pos.x = WidgetBtn.BTN_Pos.x + WidgetBtn.BTN_Padding.x;

        ToolTip.BTN_Pos.x = WidgetBtn.BTN_Pos.x - 10 - GetBtnSize(ToolTip.BTN_Text.Text, ToolTip.BTN_Padding, ToolTip.BTN_Text.Font).x;
        ToolTip.BTN_Text.Pos.x = ToolTip.BTN_Pos.x + ToolTip.BTN_Padding.x;
    }

    if (!(*State)) {
        DrawGUIButton(WidgetBtn);
        if (IMGV_GUI_ButtonHover(WidgetBtn)) {
            DrawGUIButton(ToolTip);
            DrawRectangleLines(ToolTip.BTN_Pos.x, ToolTip.BTN_Pos.y,
                ToolTip.BTN_Size.x,
                ToolTip.BTN_Size.y, RED);
        }
        if (IMGV_GUI_ButtonPressed(WidgetBtn, MOUSE_BUTTON_LEFT)) {
            *State = true;
        }
        DrawRectangleLines(WidgetBtn.BTN_Pos.x, WidgetBtn.BTN_Pos.y,
            WidgetBtn.BTN_Size.x,
            WidgetBtn.BTN_Size.y, RED);

    } else {
        Vector2 PanelPos = { .x = 0, .y = WidgetBtn.BTN_Pos.y };
        if (ori == WIDGET_RIGHT) {
            WidgetBtn.BTN_Text.Text = ">";
            WidgetBtn.BTN_Pos.x = GetScreenWidth() - WidgetBtn.BTN_Size.x - width;
            WidgetBtn.BTN_Text.Pos.x = WidgetBtn.BTN_Pos.x + WidgetBtn.BTN_Padding.x;

            ToolTip.BTN_Text.Text  = TextFormat("Collapse %s", PanelName);
            ToolTip.BTN_Size.x     = GetBtnSize(TextFormat("Collapse %s", PanelName), ToolTip.BTN_Padding, ToolTip.BTN_Text.Font).x;
            ToolTip.BTN_Pos.x = WidgetBtn.BTN_Pos.x - 10;
            ToolTip.BTN_Pos.x = WidgetBtn.BTN_Pos.x - 10 - GetBtnSize(ToolTip.BTN_Text.Text, ToolTip.BTN_Padding, ToolTip.BTN_Text.Font).x;
            ToolTip.BTN_Text.Pos.x = ToolTip.BTN_Pos.x + ToolTip.BTN_Padding.x;

            PanelPos.x = WidgetBtn.BTN_Pos.x + WidgetBtn.BTN_Size.x;
        } else {
            WidgetBtn.BTN_Pos.x = width;
            WidgetBtn.BTN_Text.Text = "<";
            WidgetBtn.BTN_Text.Pos.x = WidgetBtn.BTN_Padding.x + WidgetBtn.BTN_Pos.x;

            ToolTip.BTN_Text.Text  = TextFormat("Collapse %s", PanelName);
            ToolTip.BTN_Size.x     = GetBtnSize(TextFormat("Collapse %s", PanelName), ToolTip.BTN_Padding, ToolTip.BTN_Text.Font).x;
            ToolTip.BTN_Pos.x      = WidgetBtn.BTN_Pos.x + WidgetBtn.BTN_Size.x + 10;
            ToolTip.BTN_Text.Pos.x = ToolTip.BTN_Padding.x + ToolTip.BTN_Pos.x;
            PanelPos.x = WidgetBtn.BTN_Pos.x - width;
        }
        Pdata.DrawFn(PanelPos, width, height, font, Pdata.OtherData);

        if (IMGV_GUI_ButtonPressed(WidgetBtn, MOUSE_BUTTON_LEFT)) {
            *State = !*State;
        }
        DrawGUIButton(WidgetBtn);
        if (IMGV_GUI_ButtonHover(WidgetBtn)) {
            DrawGUIButton(ToolTip);
            DrawRectangleLines(ToolTip.BTN_Pos.x, ToolTip.BTN_Pos.y,
                ToolTip.BTN_Size.x,
                ToolTip.BTN_Size.y, RED);
        }

        DrawRectangleLines(WidgetBtn.BTN_Pos.x, WidgetBtn.BTN_Pos.y,
            WidgetBtn.BTN_Size.x,
            WidgetBtn.BTN_Size.y, RED);
    }
}

#endif // IMGV_GUI_IMPLEMENTATION
