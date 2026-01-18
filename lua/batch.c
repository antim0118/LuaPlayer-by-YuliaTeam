#include "LUA.h"

extern g2dImage **toG2D(lua_State *L, int index);
extern g2dColor *toColor(lua_State *L, int index);

#define BATCH_TYPE_TEX 1

typedef struct
{
    char type;
    g2dImage *img;
    int x, y;
    // 1+4+8 = 13 bytes
} BatchData;

static BatchData batch_data[64] = { 0 };
static int batch_lastnum = -1;

static int Batch_addDraw(lua_State *L) {
    //Batch.addDraw(tex, x, y, )
    if (lua_gettop(L) != 3)
        return luaL_error(L, "LGN.?(...) takes ? argument");

    g2dImage *img = *toG2D(L, 1);

    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);

    batch_lastnum++;
    batch_data[batch_lastnum].type = BATCH_TYPE_TEX;
    batch_data[batch_lastnum].img = img;
    batch_data[batch_lastnum].x = x;
    batch_data[batch_lastnum].y = y;

    printf("Adding batch [%d] - x:%d  y:%d \n", batch_lastnum, batch_data[batch_lastnum].x, batch_data[batch_lastnum].y);

    lua_pushnumber(L, batch_lastnum);

    return 1;
}

static int Batch_render(lua_State *L) {
    //Batch.render()

    for (size_t i = 0; i <= batch_lastnum; i++) {
        BatchData data = batch_data[i];
        // printf("drawing [%d] - x:%d  y:%d \n", i, data.x, data.y);

        if (data.type == BATCH_TYPE_TEX) {
            // g2dBeginRects(data.img);
            // // g2dSetCoordMode(AlMode);
            // // g2dSetTexLinear(linear);
            // // g2dSetTexRepeat(repeat);
            // g2dSetCoordXY(data.x, data.y);
            // // g2dSetRotation(Angle);
            // // if (color != 0)
            // //     g2dSetColor(color);
            // // g2dSetAlpha(a);
            // g2dAdd();
            // g2dEnd();
            g2dBeginRects(data.img);
            g2dSetCoordMode(G2D_UP_LEFT);
            g2dSetTexLinear(false);
            g2dSetTexRepeat(false);
            g2dSetCoordXY(data.x, data.y);
            g2dSetRotation(0);
            // g2dSetColor(0xFFFFFFFF);
            g2dSetAlpha(255);
            g2dAdd();
            g2dEnd();
        }
    }

    return 0;
}

static const luaL_Reg Batch_methods[] = {
    {"addDraw",         Batch_addDraw},
    {"render",          Batch_render},
    {0, 0}
};

int BATCH_init(lua_State *L) {
    luaL_register(L, "Batch", Batch_methods);

    return 0;
}