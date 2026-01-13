#include "LUA.h"

extern g2dImage **toG2D(lua_State *L, int index);
extern g2dColor *toColor(lua_State *L, int index);

static int LGN_exit(lua_State *L) {
    if (lua_gettop(L) != 0)
        return luaL_error(L, "LGN.exit() takes no arguments");

    return 0;
}

static int LGN_delay(lua_State *L) {
    if (lua_gettop(L) != 1)
        return luaL_error(L, "LGN.sleep(ms) takes 1 argument");

    sceKernelDelayThreadCB(luaL_checknumber(L, 1) * 1000);

    return 0;
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

void DrawSprite(g2dImage *texData, int x, int y, int originX, int originY, int xscale, int  yscale, int  rot, int  color, int  alpha) {
    // FILE *file = fopen(filename, "wb");
    // if (file == NULL) {
    //     perror("Не удалось открыть файл для записи");
    //     return;
    // }

    // fwrite(data, sizeof(unsigned char), size, file);

    // fclose(file);
}

static int LGN_drawTitle(lua_State *L) {
    int args = lua_gettop(L);
    // if(args < 3 || args > 9)
    //     return luaL_error(L, "LGN_drawTitle(....) takes 3, 4, 5, 6, 7, 8 or 9 arguments");

    g2dImage *sprTitle1 = *toG2D(L, 1);
    g2dImage *sprTitle2 = *toG2D(L, 2);
    if (!sprTitle1 || !sprTitle2)
        return luaL_error(L, "LGN_drawTitle() can't get the texture");

    int x = luaL_checknumber(L, 3);
    int y = luaL_checknumber(L, 4);
    int title2Count = luaL_checknumber(L, 5);
    // u32 color = (args >= 4 && !lua_isnil(L, 4)) ? *toColor(L, 4) : 0;
    float Angle = luaL_optnumber(L, 6, 0.0f);
    // int a = luaL_optnumber(L, 6, 255);
    // int AlMode = luaL_optnumber(L, 7, G2D_UP_LEFT);
    // bool linear = (lua_toboolean(L, 8)) ? true : false;
    // bool repeat = (lua_toboolean(L, 9)) ? true : false;

    for (size_t i1 = 0; i1 < 10; i1 += 2) {

        g2dBeginRects(sprTitle2);
        // g2dSetTexLinear(false);
        // g2dSetTexRepeat(false);
        g2dSetOriginXY(64, 16);
        g2dSetRotation(Angle);
        g2dSetScaleWH(sprTitle1->w * 2.5f, sprTitle1->h * 2.5f);
        for (size_t i2 = 0; i2 < title2Count; i2++) {
            g2dSetCoordXY(240 - x * i2, 80 - y * i2);
            // DrawSprite(, 64, 16, 2.5, 2.5, Angle, cols2[i]);

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
    {"exit",        LGN_exit},
    {"quit",        LGN_exit},
    {"sleep",       LGN_delay},
    {"draw",        LGN_draw},
    {"drawTitle",   LGN_drawTitle},
    {0, 0}
};

int LGN_init(lua_State *L) {
    luaL_register(L, "LGN", LGN_methods);

    return 0;
}