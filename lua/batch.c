#include "LUA.h"

extern g2dImage **toG2D(lua_State *L, int index);
extern g2dColor *toColor(lua_State *L, int index);

#define BATCH_MAX 128
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

static BatchData batch_data[BATCH_MAX] = { 0 }; // 5248 bytes = 5.13kb
static int batch_lastnum = -1;

int getNextIndex() {
    if (batch_lastnum >= BATCH_MAX - 1)
        return -1;
    batch_lastnum++;
    return batch_lastnum;
}

static int L_addDraw(lua_State *L) {
    if (lua_gettop(L) != 4)
        return luaL_error(L, "L_addDraw(...) takes ? argument");

    g2dImage *img = *toG2D(L, 1);

    int idx = getNextIndex();
    batch_data[idx].type = BATCH_TYPE_TEX;
    batch_data[idx].img = img;

    batch_data[idx].x = luaL_checknumber(L, 2);
    batch_data[idx].y = luaL_checknumber(L, 3);
    batch_data[idx].z = 32767 - luaL_checknumber(L, 4);

    lua_pushinteger(L, idx);

    return 1;
}

static int L_addTile(lua_State *L) {
    //Batch.addTile(tex, x, y, w, h)
    if (lua_gettop(L) != 6)
        return luaL_error(L, "L_addTile(...) takes ? argument");

    g2dImage *img = *toG2D(L, 1);

    int idx = getNextIndex();
    batch_data[idx].type = BATCH_TYPE_TILE;
    batch_data[idx].img = img;
    batch_data[idx].x = luaL_checkinteger(L, 2);
    batch_data[idx].y = luaL_checkinteger(L, 3);
    batch_data[idx].z = 32767 - luaL_checkinteger(L, 4);
    batch_data[idx].tile_w = luaL_checkinteger(L, 5);
    batch_data[idx].tile_h = luaL_checkinteger(L, 6);

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