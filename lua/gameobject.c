#include "LUA.h"

extern g2dImage **toG2D(lua_State *L, int index);

typedef struct Object
{
    int innerIndex;
    bool isUsed, isCreated;
    bool isVisible, isEnabled;
    // 8 bytes

    g2dImage *img;
    int x, y;
    int w, h;
    float speed_x, speed_y;
    int origin_x, origin_y;
    int rotation;
    // 4*10 = 40 bytes

    u32 color;
    int alpha;
    int blending_mode;
    // 4*3 = 12 bytes

    int ref_oncreate, ref_onupdate, ref_ondraw;
    // 4*3 = 12 bytes
} Object;

#define MAX_OBJECTS 1000

static Object objects[MAX_OBJECTS]; // 68 * 1000 = 68k bytes = 66.40kb

static int objects_lastnum = -1;

#define GAMEOBJECT_UNREF(refe)                 \
if (refe != -1) {                              \
    luaL_unref(L, LUA_REGISTRYINDEX, refe);    \
    refe = -1;                                 \
}


#pragma region Main

static int L_empty(lua_State *L) {
    luaL_error(L, "empty");

    return 0;
}

static int getFreeIndex() {
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        if (!objects[i].isUsed)
            return i;
    }
    return -1;
}

static int L_createObject(lua_State *L) {
    int args = lua_gettop(L);

    int index = getFreeIndex();

    if (index == -1) {
        luaL_error(L, "Objects.createObject() reached max amount of objects");
        return 0;
    }

    objects[index].isUsed = true;

    // onCreate
    if (args >= 1) {
        // *!
        if (!lua_isfunction(L, 1)) { //сверка типов
            return luaL_error(L, "1st argument != function");
        }

        if (objects[index].ref_oncreate != -1)
            luaL_unref(L, LUA_REGISTRYINDEX, objects[index].ref_oncreate);
        objects[index].ref_oncreate = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    //TODO: onUpdate/onDraw

    if (objects_lastnum < index)
        objects_lastnum = index;

    lua_pushnumber(L, index);

    return 1;
}

static void clearObject(lua_State *L, Object *obj) {
    obj->isUsed = false;
    obj->isCreated = false;
    obj->isVisible = true;
    obj->isEnabled = true;

    obj->img = NULL;
    obj->x = 0;
    obj->y = 0;
    obj->w = 0;
    obj->h = 0;
    obj->speed_x = 0.0f;
    obj->speed_y = 0.0f;
    obj->origin_x = 0;
    obj->origin_y = 0;
    obj->rotation = 0;

    obj->color = 0xFFFFFFFF; //white
    obj->alpha = 255;
    obj->blending_mode = 0;

    GAMEOBJECT_UNREF(obj->ref_oncreate);
    GAMEOBJECT_UNREF(obj->ref_onupdate);
    GAMEOBJECT_UNREF(obj->ref_ondraw);
}

static int L_clearObjects(lua_State *L) {
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        clearObject(L, &objects[i]);
    }
    objects_lastnum = -1;

    return 0;
}

static int L_process(lua_State *L) {
    float delta = 1.0f / 60.0f;

    /* Create */
    for (size_t i = 0; i < objects_lastnum; i++) {
        Object *obj = &objects[i];
        if (!obj->isUsed && !obj->isEnabled) continue;

        if (!obj->isCreated) {
            if (obj->ref_oncreate != -1) {
                lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_oncreate);
                lua_call(L, 0, 0); //0=num args   0=num returns //чекнуть тут ещё с error handler
            }
            obj->isCreated = true;
        }
    }

    /* Update */
    for (size_t i = 0; i < objects_lastnum; i++) {
        Object *obj = &objects[i];
        if (!obj->isUsed && !obj->isEnabled) continue;

        if (obj->ref_onupdate != -1) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_onupdate);
            lua_pushinteger(L, delta);
            lua_call(L, 1, 0); //1=num args   0=num returns //чекнуть тут ещё с error handler
        }

        obj->x += obj->speed_x;
        obj->y += obj->speed_y;
    }

    /* Draw */
    for (size_t i = 0; i < objects_lastnum; i++) {
        Object *obj = &objects[i];
        if (!obj->isUsed && !obj->isEnabled) continue;

        if (!obj->isVisible) continue;

        if (obj->ref_ondraw == -1) {
            // custom draw / onDraw
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_ondraw);
            lua_pushinteger(L, delta);
            lua_call(L, 1, 0); //1=num args   0=num returns //чекнуть тут ещё с error handler
        } else {
            //default draw
            if (!obj->img) continue;
            g2dBeginRects(obj->img);
            // g2dSetCoordMode(AlMode);
            // g2dSetTexLinear(linear);
            // g2dSetTexRepeat(repeat);
            g2dSetOriginXY(obj->origin_x, obj->origin_y);
            g2dSetCoordXY(obj->x, obj->y);
            // g2dSetScaleWH(w, h);
            g2dSetRotation(obj->rotation);
            // if (color != 0)
            //     g2dSetColor(color);
            // g2dSetAlpha(a);
            g2dAdd();
            g2dEnd();
        }
    }

    return 0;
}

#pragma endregion

#pragma region Setters / Getters

#define MACROS_CHECK_ARGS_AND_GET_INDEX(name, argsNum)                              \
    int args = lua_gettop(L);                                                       \
    if (args != argsNum)                                                            \
        return luaL_error(L, "Objects." #name "() takes " #argsNum " arguments");   \
    int index = luaL_checkinteger(L, 1);    


static int L_setTexture(lua_State *L) {
    MACROS_CHECK_ARGS_AND_GET_INDEX(setTexture, 2);

    g2dImage *img = *toG2D(L, 2);
    if (!img)
        return luaL_error(L, "Objects.setTexture() can't get the texture");

    objects[index].img = img;

    return 0;
}

static int L_getPos(lua_State *L) {
    MACROS_CHECK_ARGS_AND_GET_INDEX(getPos, 1);

    lua_pushnumber(L, objects[index].x);
    lua_pushnumber(L, objects[index].y);

    return 2;
}

static int L_setPos(lua_State *L) {
    MACROS_CHECK_ARGS_AND_GET_INDEX(setPos, 3);

    objects[index].x = luaL_checkinteger(L, 2);
    objects[index].y = luaL_checkinteger(L, 3);

    return 0;
}

static int L_getSize(lua_State *L) {
    MACROS_CHECK_ARGS_AND_GET_INDEX(getSize, 1);

    lua_pushnumber(L, objects[index].w);
    lua_pushnumber(L, objects[index].h);

    return 2;
}

static int L_setSize(lua_State *L) {
    MACROS_CHECK_ARGS_AND_GET_INDEX(setSize, 3);

    objects[index].w = luaL_checkinteger(L, 2);
    objects[index].h = luaL_checkinteger(L, 3);

    return 0;
}

static int L_getSpeed(lua_State *L) {
    MACROS_CHECK_ARGS_AND_GET_INDEX(getSpeed, 1);

    lua_pushnumber(L, objects[index].speed_x);
    lua_pushnumber(L, objects[index].speed_y);

    return 2;
}

static int L_setSpeed(lua_State *L) {
    MACROS_CHECK_ARGS_AND_GET_INDEX(setSpeed, 3);

    objects[index].speed_x = luaL_checkinteger(L, 2);
    objects[index].speed_y = luaL_checkinteger(L, 3);

    return 0;
}

#pragma endregion

static const luaL_Reg L_methods[] = {
    {"createObject",        L_createObject},
    {"clearObjects",        L_clearObjects},
    {"process",             L_process},

    {"setTexture",          L_setTexture},
    {"getPos",              L_getPos},
    {"setPos",              L_setPos},
    {"getSize",             L_getSize},
    {"setSize",             L_setSize},
    {"getSpeed",            L_getSpeed},
    {"setSpeed",            L_setSpeed},

    {"setCallbacks",        L_empty},
    {"setCollisions",       L_empty},
    {"calculateCollisions", L_empty},

    {0, 0}
};

static const luaL_Reg L_metamethods[] = {
    // {"__gc", L_clearObject},
    {0, 0}
};


int GAMEOBJECT_init(lua_State *L) {
    L_clearObjects(L);
    // luaL_register(L, "GameObject", L_methods);
    UserdataRegister("GameObject", L_methods, L_metamethods);

    return 0;
}

#undef GAMEOBJECT_UNREF
#undef MACROS_CHECK_ARGS_AND_GET_INDEX