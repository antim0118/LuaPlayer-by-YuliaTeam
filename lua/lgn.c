#include "LUA.h"

extern g2dImage **toG2D(lua_State *L, int index);
extern g2dColor *toColor(lua_State *L, int index);

/* (12+10+12) * 4 = 136 bytes */
static u32 colors1[10] = { 0 };
static u32 colors2[10] = { 0 };
static u32 colors3[11] = { 0 };

static u32 colorLerp(u32 col1, u32 col2, float val) {
    int r1 = G2D_GET_R(col1), g1 = G2D_GET_G(col1), b1 = G2D_GET_B(col1);
    int r2 = G2D_GET_R(col2), g2 = G2D_GET_G(col2), b2 = G2D_GET_B(col2);

    // int r = (r2 - r1) * val + r1;
    // printf("lerp 2\n");
    // int g = (g2 - g1) * val + g1;
    // printf("lerp 3\n");
    // int b = (b2 - b1) * val + b1;

    int ival = (int)(val * 255.0f);

    int r = ((r2 - r1) * ival + r1 * 255) / 255;
    int g = ((g2 - g1) * ival + g1 * 255) / 255;
    int b = ((b2 - b1) * ival + b1 * 255) / 255;

    r = (r < 0 ? 0 : (r > 255 ? 255 : r));
    g = (g < 0 ? 0 : (g > 255 ? 255 : g));
    b = (b < 0 ? 0 : (b > 255 ? 255 : b));

    return G2D_RGBA(r, g, b, 255);
}

#define COLORS2_LAST 7

static void generateColors() {
    u32 col1 = G2D_RGBA(255, 53, 244, 255), col2 = G2D_RGBA(107, 230, 255, 255);
    for (size_t i = 1; i <= 10; i++) {
        colors1[i - 1] = colorLerp(col1, col2, (float)i / 10.0f);
    }

    u32 c_red = G2D_RGBA(255, 0, 0, 255), c_white = G2D_RGBA(255, 255, 255, 255);
    for (size_t i = 1; i <= COLORS2_LAST; i++) {
        colors2[i - 1] = colorLerp(c_red, c_white, (float)i / 10.0f);
    }

    col1 = G2D_RGBA(188, 57, 211, 255), col2 = c_white;
    for (size_t i = 0; i < 11; i++) {
        colors3[i] = colorLerp(col1, col2, (float)i / 10.0f);
    }
}

static int LGN_draw(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 3 || args > 9)
        return luaL_error(L, "Image.draweasy(texture, x, y, [color], [rotation], [alpha], [alMode], [GU_LINEAR], [GU_REPEAT]) takes 3, 4, 5, 6, 7, 8 or 9 arguments");

    g2dImage *img = *toG2D(L, 1);
    if (!img)
        return luaL_error(L, "Image.draweasy() can't get the texture");

    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    u32 color = (args >= 4 && !lua_isnil(L, 4)) ? *toColor(L, 4) : 0;
    float Angle = luaL_optnumber(L, 5, 0.0f);
    float origin_x = luaL_optnumber(L, 6, 0.0f);
    float origin_y = luaL_optnumber(L, 7, 0.0f);
    // int a = luaL_optnumber(L, 6, 255);
    // int AlMode = luaL_optnumber(L, 7, G2D_UP_LEFT);
    // bool linear = (lua_toboolean(L, 8)) ? true : false;
    // bool repeat = (lua_toboolean(L, 9)) ? true : false;

    g2dBeginRects(img);
    // g2dSetCoordMode(AlMode);
    // g2dSetTexLinear(linear);
    // g2dSetTexRepeat(repeat);
    g2dSetOriginXY(origin_x, origin_y);
    g2dSetCoordXY(x, y);
    // g2dSetScaleWH(w, h);
    g2dSetRotation(Angle);
    if (color != 0)
        g2dSetColor(color);
    // g2dSetAlpha(a);
    g2dAdd();
    g2dEnd();

    g2dSetOriginXY(0, 0);

    return 0;
}

static int LGN_drawMenuTitle(lua_State *L) {
    // int args = lua_gettop(L);
    // if(args != 6)
    //     return luaL_error(L, "LGN_drawMenuTitle(....) takes 6 args");

    g2dImage *sprTitle1 = *toG2D(L, 1);
    g2dImage *sprTitle2 = *toG2D(L, 2);
    if (!sprTitle1 || !sprTitle2)
        return luaL_error(L, "LGN_drawMenuTitle() can't get the texture");

    float x = luaL_checknumber(L, 3);
    float y = luaL_checknumber(L, 4);
    
    int title2Count = luaL_checknumber(L, 5);
    if (title2Count < 1) title2Count = 1;
    if (title2Count > COLORS2_LAST) title2Count = COLORS2_LAST;
    
    float Angle = luaL_optnumber(L, 6, 0.0f);

    g2dBeginRects(sprTitle1);
    g2dSetOriginXY(64 * 2.5f, 16 * 2.5f);
    g2dSetRotation(Angle);
    g2dSetScaleWH(sprTitle1->w * 2.5f, sprTitle1->h * 2.5f);
    for (size_t i1 = 1; i1 <= 10; i1 += 2) {
        g2dSetCoordXYZ(240 - x * i1, 80 - y * i1, 21.0f - i1);
        g2dSetColor(colors1[i1 - 1]);
        g2dAdd();
    }
    g2dEnd();

    g2dBeginRects(sprTitle2);
    // g2dSetTexLinear(false);
    // g2dSetTexRepeat(false);
    g2dSetOriginXY(64 * 2.5f, 16 * 2.5f);
    g2dSetRotation(Angle);
    g2dSetScaleWH(sprTitle2->w * 2.5f, sprTitle2->h * 2.5f);
    for (size_t i2 = 1; i2 <= title2Count; i2++) {
        g2dSetCoordXYZ(240 - x * i2, 80 - y * i2, 20.0f - i2);
        g2dSetColor(colors2[i2 - 1]);
        g2dAdd();
    }
    g2dEnd();

    return 0;
}
            g2dAdd();
        }
        g2dEnd();

        g2dBeginRects(sprTitle1);
        g2dSetRotation(Angle);
        // DrawSprite(sprTitle1, 240 - x * i, 80 - y * i, 64, 16, 2.5, 2.5, Angle, cols1[i]);
        g2dEnd();

        // g2dSetCoordMode(AlMode);
        // g2dSetTexLinear(linear);
        // g2dSetTexRepeat(repeat);
        // g2dSetCoordXY(x, y);
        // g2dSetScaleWH(w, h);
        // g2dSetRotation(Angle);
        // if (color != 0)
        //     g2dSetColor(color);
        // g2dSetAlpha(a);
        // g2dAdd();
        // g2dEnd();
    }

    return 0;
}

static const luaL_Reg LGN_methods[] = {
    {"draw",            LGN_draw},
    {"drawMenuTitle",   LGN_drawMenuTitle},
    {0, 0}
};

int LGN_init(lua_State *L) {
    generateColors();

    luaL_register(L, "LGN", LGN_methods);

    return 0;
}