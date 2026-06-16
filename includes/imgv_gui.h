#pragma once

#ifndef IMGV_GUI_H
#define IMGV_GUI_H

#include <stdio.h>
#include <raylib.h>

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

#define IMGV_GUI_IMPLEMENTATION // For debugging/developing

#ifdef IMGV_GUI_IMPLEMENTATION

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
