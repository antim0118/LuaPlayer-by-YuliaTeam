#include "LUA.h"

extern g2dImage **toG2D(lua_State *L, int index);

typedef struct Object
{
    g2dImage *img;
    int x, y;
    float velocity_x, velocity_y;
    int origin_x, origin_y;
    int rotation;
    // 4+8+8+8+4 = 32 bytes
} Object;

#define MAX_OBJECTS 512

static Object objects[MAX_OBJECTS] = { // 32*512 = 16384 bytes = 16 kb
    NULL,
    0, 0,
    0.0f, 0.0f,
    0, 0,
    0
};

static int L_lastnum = -1;

static int L_empty(lua_State *L) {
    luaL_error(L, "gbcmrb!");

    return 0;
}

static int L_createObject(lua_State *L) {
    objects_lastnum++;

    if (objects_lastnum >= MAX_OBJECTS) {
        luaL_error(L, "Objects.createObject() reached max amount of objects");
        return 0;
    }

    lua_pushnumber(L, objects_lastnum);

    return 1;
}

static int L_clearObjects(lua_State *L) {
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        objects[i].img = NULL;
        objects[i].x = 0;
        objects[i].y = 0;
        objects[i].velocity_x = 0.0f;
        objects[i].velocity_y = 0.0f;
        objects[i].origin_x = 0;
        objects[i].origin_y = 0;
        objects[i].rotation = 0;
    }

    return 0;
}

static int L_process(lua_State *L) {
    for (size_t i = 0; i < objects_lastnum; i++) {
        objects[i].x += objects[i].velocity_x;
        objects[i].y += objects[i].velocity_y;

        Object obj = objects[i];

        if (!obj.img) continue;

        g2dBeginRects(obj.img);
        // g2dSetCoordMode(AlMode);
        // g2dSetTexLinear(linear);
        // g2dSetTexRepeat(repeat);
        g2dSetOriginXY(obj.origin_x, obj.origin_y);
        g2dSetCoordXY(obj.x, obj.y);
        // g2dSetScaleWH(w, h);
        g2dSetRotation(obj.rotation);
        // if (color != 0)
        //     g2dSetColor(color);
        // g2dSetAlpha(a);
        g2dAdd();
        g2dEnd();
    }

    return 0;
}

static int L_setTexture(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 2)
        return luaL_error(L, "Objects.setTexture(index, tex) takes 2 arguments");

    int index = luaL_checkinteger(L, 1);

    g2dImage *img = *toG2D(L, 2);
    if (!img)
        return luaL_error(L, "Objects.setTexture() can't get the texture");

    objects[index].img = img;

    return 0;
}

static int L_getPos(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1)
        return luaL_error(L, "Objects.getPos(index) takes 1 arguments");

    int index = luaL_checkinteger(L, 1);
    lua_pushnumber(L, objects[index].x);
    lua_pushnumber(L, objects[index].y);

    return 2;
}

static int L_setPos(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 3)
        return luaL_error(L, "Objects.setPos(index, x, y) takes 3 arguments");

    int index = luaL_checkinteger(L, 1);
    objects[index].x = luaL_checkinteger(L, 2);
    objects[index].y = luaL_checkinteger(L, 3);

    return 0;
}

static int L_getVelocity(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1)
        return luaL_error(L, "Objects.getVelocity(index) takes 1 arguments");

    int index = luaL_checkinteger(L, 1);
    lua_pushnumber(L, objects[index].velocity_x);
    lua_pushnumber(L, objects[index].velocity_y);

    return 2;
}

static int L_setVelocity(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 3)
        return luaL_error(L, "Objects.setPos(index, velocityX, velocityY) takes 3 arguments");

    int index = luaL_checkinteger(L, 1);
    objects[index].velocity_x = luaL_checkinteger(L, 2);
    objects[index].velocity_y = luaL_checkinteger(L, 3);

    return 0;
}

static const luaL_Reg L_methods[] = {
    {"createObject",        L_createObject},
    {"clearObjects",        L_clearObjects},
    {"process",             L_process},

    {"setTexture",          L_setTexture},
    {"getPos",              L_getPos},
    {"setPos",              L_setPos},
    {"getVelocity",         L_getVelocity},
    {"setVelocity",         L_setVelocity},

    {"setCallbacks",       L_empty},
    {"setCollisions",       L_empty},
    {"calculateCollisions", L_empty},

    {0, 0}
};

int GAMEOBJECT_init(lua_State *L) {
    // luaL_register(L, "GameObject", L_methods);

    luaL_newmetatable(L, "_GameObject");

    lua_pushvalue(L, -1); 
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, L_setPos);
    lua_setfield(L, -2, "SetPos");

    lua_newtable(L);
    lua_pushcfunction(L, L_NewObject);
    lua_setfield(L, -2, "new");
    lua_setglobal(L, "GameObject");

    return 0;
}