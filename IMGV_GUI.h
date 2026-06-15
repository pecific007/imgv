#pragma once

#ifndef IMGV_GUI_H
#define IMGV_GUI_H

#include <stdio.h>
#include <raylib.h>

typedef enum {
    BTN_QUIT, // Quit button
    BTN_CONT, // Continue button
} BTN_Kind;


#define BUTTON_BOTTOM_Y(BtnPaddingY, PaddingY) GetScreenHeight() - (BtnPaddingY*2 + fonts.Size30.baseSize + PaddingY)
#define BUTTON_RIGHT_X(BtnPaddingX, PaddingX, text, font) GetScreenWidth() - (BtnPaddingX*2 + (MeasureTextEx(font, text, font.baseSize, 1).x) + PaddingX)

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
    BTN_Kind      BTN_Kind;
    IMGV_BTN_TEXT BTN_Text;
} IMGV_GUI_BTN;

IMGV_GUI_BTN CreateGUIButton(const char *text,
    Vector2 pos, Vector2 padding, Color BtnColor,
    Color TextColor, Font font, BTN_Kind BtnType);
void DrawGUIButton(IMGV_GUI_BTN Button);
bool IMGVGUIButtonPressed(IMGV_GUI_BTN Button);
bool IMGV_GUI_ButtonHover(IMGV_GUI_BTN Button);
bool IMGV_GUI_ButtonPressed(IMGV_GUI_BTN Button, MouseButton click);


#endif // IMGV_GUI_H

// #define IMGV_GUI_IMPLEMENTATION

#ifdef IMGV_GUI_IMPLEMENTATION

IMGV_GUI_BTN CreateGUIButton(const char *text,
    Vector2 pos, Vector2 padding, Color BtnColor,
    Color TextColor, Font font, BTN_Kind BtnType)
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
        .BTN_Kind = BtnType,
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
    // printf("\n%i %f %f\n", (MousePos.y >= (Button.BTN_Pos.y + Button.BTN_Size.y)), (Button.BTN_Pos.y + Button.BTN_Size.y), GetMousePosition().y);
    // ^ for debugging

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
    if (IsMouseButtonDown(click)) {
        return true;
    }
    return false;
}

#endif // IMGV_GUI_IMPLEMENTATION
