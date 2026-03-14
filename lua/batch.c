#include "LUA.h"

extern g2dImage **toG2D(lua_State *L, int index);
extern g2dColor *toColor(lua_State *L, int index);

#define BATCH_MAX 64
typedef enum
{
    BATCH_TYPE_TEX,
    BATCH_TYPE_TILE
} BATCH_TYPES;

typedef struct
{
    char type;
    g2dImage *img;
    int x, y, z;
    int tile_w, tile_h;
    // 1+4+8+8 = 21 bytes
} BatchData;

static BatchData batch_data[BATCH_MAX] = { 0 };
static int batch_lastnum = -1;

int getNextIndex() {
    if (batch_lastnum >= BATCH_MAX - 1)
        return -1;
    batch_lastnum++;
    return batch_lastnum;
}

static int L_addDraw(lua_State *L) {
    //Batch.addDraw(tex, x, y, )
    if (lua_gettop(L) != 4)
        return luaL_error(L, "Batch_addDraw(...) takes ? argument");

    g2dImage *img = *toG2D(L, 1);

    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    int z = luaL_checknumber(L, 4);

    int idx = getNextIndex();
    batch_data[idx].type = BATCH_TYPE_TEX;
    batch_data[idx].img = img;
    batch_data[idx].x = x;
    batch_data[idx].y = y;
    batch_data[idx].z = z;

    printf("Batch_addDraw [%d] - x:%d  y:%d z:%d \n", idx, x, y, z);

    lua_pushinteger(L, idx);

    return 1;
}

static int L_addTile(lua_State *L) {
    //Batch.addTile(tex, x, y, w, h)
    if (lua_gettop(L) != 6)
        return luaL_error(L, "Batch_addDraw(...) takes ? argument");

    g2dImage *img = *toG2D(L, 1);

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int z = luaL_checkinteger(L, 4);
    int w = luaL_checkinteger(L, 5);
    int h = luaL_checkinteger(L, 6);

    int idx = getNextIndex();
    batch_data[idx].type = BATCH_TYPE_TILE;
    batch_data[idx].img = img;
    batch_data[idx].x = x;
    batch_data[idx].y = y;
    batch_data[idx].z = 32767 - z;
    batch_data[idx].tile_w = w;
    batch_data[idx].tile_h = h;

    lua_pushinteger(L, idx);

    return 1;
}

static int L_render(lua_State *L) {
    if (batch_lastnum == -1)
        return 0;

    for (size_t i = 0; i <= batch_lastnum; i++) {
        BatchData *data = &batch_data[i];
        // printf("drawing [%d] - x:%d  y:%d \n", i, data.x, data.y);

        g2dBeginRects(data->img);

        if (data->type == BATCH_TYPE_TEX) {
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

            g2dSetTexLinear(false);
            g2dSetTexRepeat(false);
            g2dSetCoordXYZ(data->x, data->y, data->z);
            // g2dSetRotation(0);
            // g2dSetColor(0xFFFFFFFF);
            // g2dSetAlpha(255);
            g2dAdd();
        } else if (data->type == BATCH_TYPE_TILE) {
            g2dBeginRects(data->img);
            g2dSetTexLinear(false);
            g2dSetTexRepeat(true);
            g2dSetCoordXYZ(data->x, data->y, data->z);
            g2dSetCropXY(0, 0);
            g2dSetCropWH(data->tile_w, data->tile_h);
            g2dSetScaleWH(data->tile_w, data->tile_h);
            g2dAdd();
        }

        g2dEnd();
    }

    return 0;
}

static int L_clear(lua_State *L) {
    batch_lastnum = -1;

    return 0;
}

static int L_getCount(lua_State *L) {
    lua_pushnumber(L, batch_lastnum + 1);

    return 1;
}

static const luaL_Reg L_methods[] = {
    {"addDraw",         L_addDraw},
    {"addTile",         L_addTile},
    {"render",          L_render},
    {"clear",           L_clear},
    {"getCount",        L_getCount},
    {0, 0}
};

int BATCH_init(lua_State *L) {
    luaL_register(L, "Batch", L_methods);

    return 0;
}