/**
    Internal graphic functions of Imhotep Engine
    File - g2d.c
    *Made by Kodilo*
*/
#include "g2d.h"

int g2D_draw(g2dImage tex, int x, int y, int w, int h, g2dColor color, int srcx, int srcy, int srcw, int srch, int r, int a, g2dCoord_Mode mode)
{
    g2dBeginRects(tex);
    g2dSetCoordMode(mode);
    g2dSetCoordXY(x,y);
    g2dSetCropXY(srcx,srcy);
    g2dSetCropWH(srcw,srch);
    g2dSetScaleWH(w,h);
    g2dSetRotation(r);
    g2dSetColor(color);
    g2dSetAlpha(a);
    g2dAdd();
    g2dEnd();

    return 0;
}

int g2D_draweasy(g2dImage tex, int x, int y, g2dColor color, int r, int a, g2dCoord_Mode mode)
{
    g2dBeginRects(tex);
    g2dSetCoordMode(mode);
    g2dSetCoordXY(x,y);
    g2dSetRotation(r);
    g2dSetColor(color);
    g2dSetAlpha(a);
    g2dAdd();
    g2dEnd();

    return 0;
}

int g2D_drawRect(int x, int y, int w, int h, g2dColor color, int r, int a)
{
    g2dBeginRects(NULL);
    g2dSetCoordXY(x,y);
    g2dSetRotation(r);
    g2dSetColor(color);
    g2dSetAlpha(a);
    g2dAdd();
    g2dEnd();

    return 0;
}

int g2D_drawCircle(int x, int y, int r, g2dColor color, int a)
{
    return 0;
}