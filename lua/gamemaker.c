#include "LUA.h"

extern g2dImage **toG2D(lua_State *L, int index);
extern g2dColor *toColor(lua_State *L, int index);
extern intraFont **getintraFont(lua_State *L, int index);

static u32 color = 0;
static int blend_mode = -1;
static int rotation = 0;
static int alpha = 255;
static bool linear = false;
static intraFont *font = NULL;

static void drawRect(int x1, int y1, int x2, int y2, g2dColor col1, g2dColor col2, g2dColor col3, g2dColor col4) {
    g2dSetCoordXY(x1, y1); g2dSetColor(col1); g2dAdd(); // TL
    g2dSetCoordXY(x2, y1); g2dSetColor(col2); g2dAdd(); // TR
    g2dSetCoordXY(x2, y2); g2dSetColor(col3); g2dAdd(); // BR
    g2dSetCoordXY(x1, y2); g2dSetColor(col4); g2dAdd(); // BL
}

static intraFont *getFont(lua_State *L, int index) {
    if (lua_isnil(L, index))
        return luaFont;

    intraFont *font = *getintraFont(L, index);
    if (font == NULL)
        return luaFont;

    return font;
}

static int L_empty(lua_State *L) {
    luaL_error(L, "empty");

    return 0;
}

#pragma region Setters
static int L_draw_set_blend_mode(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1)
        return luaL_error(L, ".draw_set_blend_mode(mode) takes 1 argument");

    blend_mode = luaL_checknumber(L, 1);

    return 0;
}

static int L_draw_set_color(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1)
        return luaL_error(L, ".draw_set_color(color) takes 1 argument");

    color = *toColor(L, 1);

    return 0;
}

static int L_draw_set_font(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1)
        return luaL_error(L, ".draw_set_font(font) takes 1 argument");

    font = getFont(L, 1);

    return 0;
}

static int L_draw_set_alpha(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1)
        return luaL_error(L, ".draw_set_alpha(alpha) takes 1 argument");

    alpha = setInterval(luaL_checknumber(L, 1) * 255, 0, 255);

    return 0;
}
#pragma endregion

static int L_draw(lua_State *L) {
    int args = lua_gettop(L);

    g2dImage *img = *toG2D(L, 1);
    if (!img)
        return luaL_error(L, ".draw() can't get the texture");

    int x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3), z = luaL_optnumber(L, 4, 0);
    int w = luaL_optnumber(L, 5, img->w), h = luaL_optnumber(L, 6, img->h);
    int srcx = luaL_optnumber(L, 7, 0), srcy = luaL_optnumber(L, 8, 0);
    int srcw = luaL_optnumber(L, 9, img->w), srch = luaL_optnumber(L, 10, img->h);
    float origin_x = luaL_optnumber(L, 11, 0);
    float origin_y = luaL_optnumber(L, 12, 0);
    bool repeat = (lua_toboolean(L, 13)) ? true : false;

    g2dBeginRects(img);
    g2dSetOriginXY(origin_x, origin_y);
    g2dSetTexLinear(linear);
    g2dSetTexRepeat(repeat);
    g2dSetCoordXYZ(x, y, z);
    g2dSetCropXY(srcx, srcy);
    g2dSetCropWH(srcw, srch);
    g2dSetScaleWH(w, h);
    g2dSetRotation(-rotation);
    if (color != 0)
        g2dSetColor(color);
    g2dSetAlpha(alpha);
    if (blend_mode != -1)
        g2dSetTexBlendMode(blend_mode);
    g2dAdd();
    g2dEnd();

    return 0;
}

static int L_draw_sprite(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 3)
        return luaL_error(L, ".draw_sprite(sprite, x, y) takes ... arguments");

    g2dImage *img = *toG2D(L, 1);
    if (!img)
        return luaL_error(L, ".draw_sprite() can't get the texture");

    int x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);

    g2dBeginRects(img);
    g2dSetTexLinear(linear);
    g2dSetCoordXY(x, y);
    g2dSetScaleWH(img->w, img->h);
    g2dSetRotation(-rotation);
    g2dSetColor(color);
    g2dSetAlpha(alpha);
    if (blend_mode != -1)
        g2dSetTexBlendMode(blend_mode);
    g2dAdd();
    g2dEnd();

    return 0;
}

static int L_draw_sprite_ext(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 10)
        return luaL_error(L, ".draw_sprite_ext(sprite, x, y, xscale, yscale, rot, color, alpha, originX, originY) takes ... arguments");

    g2dImage *img = *toG2D(L, 1);
    if (!img)
        return luaL_error(L, ".draw_sprite_ext() can't get the texture");

    int x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);
    float xscale = luaL_checknumber(L, 4), yscale = luaL_checknumber(L, 5);
    float rot = luaL_checknumber(L, 6);
    u32 color = *toColor(L, 7);
    int alpha = luaL_checknumber(L, 8);
    float origin_x = luaL_checknumber(L, 9);
    float origin_y = luaL_checknumber(L, 10);

    g2dBeginRects(img);
    g2dSetTexLinear(linear);
    g2dSetOriginXY(origin_x, origin_y);
    g2dSetCoordXY(x, y);
    g2dSetScaleWH(img->w * xscale, img->h * yscale);
    g2dSetRotation(-rot);
    g2dSetColor(color);
    g2dSetAlpha(alpha);
    if (blend_mode != -1)
        g2dSetTexBlendMode(blend_mode);
    g2dAdd();
    g2dEnd();

    return 0;
}

static int L_draw_sprite_general(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 15)
        return luaL_error(L, ".draw_sprite_general(img, left, top, width, height, x, y, xscale, yscale, rot, c1, c2, c3, c4, alpha) takes ... arguments");

    g2dImage *img = *toG2D(L, 1);
    if (!img)
        return luaL_error(L, ".draw_sprite_general() can't get the texture");

    int left = luaL_checknumber(L, 2), top = luaL_checknumber(L, 3);
    int w = luaL_checknumber(L, 4), h = luaL_checknumber(L, 5);
    int x = luaL_checknumber(L, 6), y = luaL_checknumber(L, 7);
    float xscale = luaL_checknumber(L, 8), yscale = luaL_checknumber(L, 9);
    float rot = luaL_checknumber(L, 10);
    u32 c1 = *toColor(L, 11);
    u32 c2 = *toColor(L, 12);
    u32 c3 = *toColor(L, 13);
    u32 c4 = *toColor(L, 14);
    int alpha = luaL_checknumber(L, 15);

    g2dBeginQuads(img);
    if (blend_mode != -1)
        g2dSetTexBlendMode(blend_mode);
    g2dSetCropXY(left, top);
    g2dSetCropWH(w, h);
    g2dSetTexLinear(linear);
    // g2dSetCoordXY(x, y);
    g2dSetScaleWH(w * xscale, h * yscale);
    g2dSetAlpha(alpha);
    g2dSetRotation(-rot);
    g2dSetCoordXY(x, y);     g2dSetColor(c1); g2dAdd(); // TL
    g2dSetCoordXY(x + w, y);     g2dSetColor(c2); g2dAdd(); // TR
    g2dSetCoordXY(x + w, y + h); g2dSetColor(c3); g2dAdd(); // BR
    g2dSetCoordXY(x, y + h); g2dSetColor(c4); g2dAdd(); // BL

    g2dEnd();

    return 0;
}

static int L_draw_sprite_tiled(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 3)
        return luaL_error(L, ".draw_sprite_tiled(img, x, y) takes ... arguments");

    g2dImage *img = *toG2D(L, 1);
    if (!img)
        return luaL_error(L, ".draw_sprite_tiled() can't get the texture");

    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);

    g2dBeginQuads(img);
    if (blend_mode != -1)
        g2dSetTexBlendMode(blend_mode);
    g2dSetCropXY(x, y);
    g2dSetCropWH(480, 272);
    g2dSetTexLinear(linear);
    g2dSetTexRepeat(true);
    g2dSetCoordXY(0, 0);
    g2dSetScaleWH(480, 272);
    g2dSetAlpha(alpha);
    g2dSetRotation(-rotation);
    g2dSetColor(color);
    g2dAdd();

    g2dEnd();

    return 0;
}

static int L_draw_rectangle_color(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 9)
        return luaL_error(L, ".draw_rectangle_color(x1, y1, x2, y2, col1, col2, col3, col4, outline) takes 9 arguments");

    int x1 = luaL_checknumber(L, 1);
    int y1 = luaL_checknumber(L, 2);
    int x2 = luaL_checknumber(L, 3);
    int y2 = luaL_checknumber(L, 4);
    u32 col1 = *toColor(L, 5);
    u32 col2 = *toColor(L, 6);
    u32 col3 = *toColor(L, 7);
    u32 col4 = *toColor(L, 8);
    bool outline = (lua_toboolean(L, 9)) ? true : false;

    if (outline)
        return luaL_error(L, ".draw_rectangle_color(...) - outline is not implemented xd");

    g2dBeginQuads(NULL);
    if (blend_mode != -1) g2dSetTexBlendMode(blend_mode);

    drawRect(x1, y1, x2, y2, col1, col2, col3, col4);

    g2dEnd();

    return 0;
}

static int L_draw_circle_color(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 6)
        return luaL_error(L, ".draw_circle_color(x, y, r, col1, col2, outline) takes 6 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int r = luaL_checknumber(L, 3);
    u32 col1 = *toColor(L, 4);
    u32 col2 = *toColor(L, 5);
    bool outline = (lua_toboolean(L, 6)) ? true : false;

    if (outline)
        return luaL_error(L, ".draw_circle_color(...) - outline is not implemented xd");

    g2dBeginQuads(NULL);
    if (blend_mode != -1) g2dSetTexBlendMode(blend_mode);

    //рисуем 4 прямоугольника вместо круга хд
    drawRect(x - r, y - r, x, y, col2, col2, col1, col2);
    drawRect(x + r, y - r, x, y, col2, col2, col1, col2);
    drawRect(x - r, y + r, x, y, col2, col2, col1, col2);
    drawRect(x + r, y + r, x, y, col2, col2, col1, col2);

    g2dEnd();

    return 0;
}

static int L_draw_text(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 3)
        return luaL_error(L, ".draw_text(x, y, text) takes 3 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char *text = luaL_checkstring(L, 3);
    float size = 1;
    float alMode = INTRAFONT_ALIGN_LEFT;

    intraFontSetStyle(font, size, color, 0, rotation, alMode);
    intraFontActivate(font, linear);

    int viewX = g2dGetCameraX();
    int viewY = g2dGetCameraY();
    intraFontPrint(font, x, y + intraFontTextHeight(font), text);

    return 0;
}

static const luaL_Reg L_methods[] = {
    {"draw",                    L_draw},
    {"draw_set_blend_mode",     L_draw_set_blend_mode},
    {"draw_set_color",          L_draw_set_color},
    {"draw_set_font",           L_draw_set_font},
    {"draw_set_alpha",          L_draw_set_alpha},
    {"draw_sprite",             L_draw_sprite},
    {"draw_sprite_ext",         L_draw_sprite_ext},
    {"draw_sprite_general",     L_draw_sprite_general},
    {"draw_sprite_tiled",       L_draw_sprite_tiled},
    {"draw_rectangle_color",    L_draw_rectangle_color},
    {"draw_circle_color",       L_draw_circle_color},
    {"draw_text",               L_draw_text},

    {0, 0}
};


int GAMEMAKER_init(lua_State *L) {
    font = luaFont;
    luaL_register(L, "gm", L_methods);
    return 0;
}
