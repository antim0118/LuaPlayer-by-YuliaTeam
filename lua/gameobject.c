#include "LUA.h"

extern g2dImage **toG2D(lua_State *L, int index);

typedef struct Object
{
    bool isUsed, isCreated;
    bool isVisible, isEnabled;
    // 4 bytes

    g2dImage *img;
    int x, y;
    int w, h;
    int crop_x, crop_y, crop_w, crop_h;
    float speed_x, speed_y;
    float origin_x, origin_y;
    int rotation;
    // 4*14 = 56 bytes

    u32 color;
    int alpha;
    int blending_mode;
    bool use_camera;
    // 4*3 + 1 = 13 bytes

    int ref_oncreate, ref_onupdate, ref_ondraw;
    // 4*3 = 12 bytes
} Object;

#define MAX_OBJECTS 2000

static Object objects[MAX_OBJECTS]; // 85 * 2000 = 170k bytes = 166.02kb

static int objects_lastidx = -1;

#define GAMEOBJECT_REF(luaIndex, refProp)                           \
if (args >= luaIndex) {                                             \
    if (!lua_isfunction(L, luaIndex))                               \
        return luaL_error(L, "argument " #luaIndex " != function"); \
    if (refProp != -1)                                              \
        luaL_unref(L, LUA_REGISTRYINDEX, refProp);                  \
    lua_pushvalue(L, luaIndex);                                     \
    refProp = luaL_ref(L, LUA_REGISTRYINDEX);                       \
}

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
    if (objects_lastidx + 1 > MAX_OBJECTS)
        return -1;
    return objects_lastidx + 1;
    // порядок отрисовки может сломаться
    // for (size_t i = 0; i < MAX_OBJECTS; i++) {
    //     if (!objects[i].isUsed)
    //         return i;
    // }
    // return -1;
}

static int getObjectCount() {
    return objects_lastidx + 1;
}

static int L_createObject(lua_State *L) {
    int args = lua_gettop(L);

    int index = getFreeIndex();

    if (index == -1) {
        luaL_error(L, "Objects.createObject() reached max amount of objects");
        return 0;
    }

    objects[index].isUsed = true;

    GAMEOBJECT_REF(1, objects[index].ref_oncreate); // onCreate
    GAMEOBJECT_REF(2, objects[index].ref_onupdate); // onUpdate
    GAMEOBJECT_REF(3, objects[index].ref_ondraw); // onDraw

    if (objects_lastidx < index)
        objects_lastidx = index;

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
    obj->crop_x = 0;
    obj->crop_y = 0;
    obj->crop_w = 0;
    obj->crop_h = 0;
    obj->speed_x = 0.0f;
    obj->speed_y = 0.0f;
    obj->origin_x = 0.0f;
    obj->origin_y = 0.0f;
    obj->rotation = 0;

    obj->color = 0xFFFFFFFF; //white
    obj->alpha = 255;
    obj->blending_mode = -1;
    obj->use_camera = false;

    GAMEOBJECT_UNREF(obj->ref_oncreate);
    GAMEOBJECT_UNREF(obj->ref_onupdate);
    GAMEOBJECT_UNREF(obj->ref_ondraw);
}

clock_t clock_delta;

static int L_clearObjects(lua_State *L) {
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        clearObject(L, &objects[i]);
    }
    objects_lastidx = -1;
    clock_delta = clock();

    return 0;
}

static int L_process(lua_State *L) {
    clock_t clock_delta_now = clock();
    float delta = (float)(clock_delta_now - clock_delta) / CLOCKS_PER_SEC;
    clock_delta = clock_delta_now;

    /* Create */
    for (size_t i = 0; i <= objects_lastidx; i++) {
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
    for (size_t i = 0; i <= objects_lastidx; i++) {
        Object *obj = &objects[i];
        if (!obj->isUsed && !obj->isEnabled) continue;

        if (obj->ref_onupdate != -1) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_onupdate);
            lua_pushnumber(L, delta);
            lua_call(L, 1, 0); //1=num args   0=num returns //чекнуть тут ещё с error handler
        }

        obj->x += obj->speed_x;
        obj->y += obj->speed_y;
    }

    /* Draw */
    bool usedCamera = g2dGetUseCamera();
    for (size_t i = 0; i <= objects_lastidx; i++) {
        Object *obj = &objects[i];
        if (!obj->isUsed && !obj->isEnabled) continue;

        if (!obj->isVisible) continue;

        if (obj->ref_ondraw != -1) {
            // custom draw / onDraw
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_ondraw);
            lua_pushnumber(L, delta);
            g2dSetUseCamera(obj->use_camera);
            lua_call(L, 1, 0); //1=num args   0=num returns //чекнуть тут ещё с error handler
        } else {
            //default draw
            if (!obj->img) continue;
            g2dBeginRects(obj->img);
            g2dSetUseCamera(obj->use_camera);
            // g2dSetCoordMode(AlMode);
            // g2dSetTexLinear(linear);
            // g2dSetTexRepeat(repeat);
            g2dSetOriginXY(obj->origin_x, obj->origin_y);
            g2dSetCoordXY(obj->x, obj->y);
            if (obj->crop_w != 0 && obj->crop_h != 0) { //TODO: может быть такое, что текстура не поменяется и кроп останется от прошлого объекта! сделать else
                g2dSetCropXY(obj->crop_x, obj->crop_y);
                g2dSetCropWH(obj->crop_w, obj->crop_h);
            }
            g2dSetScaleWH(obj->w, obj->h);
            g2dSetRotation(obj->rotation);
            if (obj->color != 0)
                g2dSetColor(obj->color);
            g2dSetAlpha(obj->alpha);
            g2dAdd();
            g2dEnd();
        }
    }
    g2dSetUseCamera(usedCamera);

    return 0;
}

#pragma endregion

#pragma region Setters / Getters

#define CHECK_ARGS_AND_GET_INDEX(name, argsNum)                              \
    int args = lua_gettop(L);                                                       \
    if (args != argsNum)                                                            \
        return luaL_error(L, "Objects." #name "() takes " #argsNum " arguments");   \
    int index = luaL_checkinteger(L, 1);    

static int L_getObjectCount(lua_State *L) {
    lua_pushnumber(L, getObjectCount());

    return 1;
}

static int L_setTexture(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setTexture, 2);

    g2dImage *img = *toG2D(L, 2);
    if (!img)
        return luaL_error(L, "Objects.setTexture() can't get the texture");

    objects[index].img = img;
    objects[index].w = img->w;
    objects[index].h = img->h;

    return 0;
}

static int L_setTextureCrop(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setTextureCrop, 5);

    objects[index].crop_x = luaL_checkinteger(L, 2);
    objects[index].crop_y = luaL_checkinteger(L, 3);
    objects[index].crop_w = luaL_checkinteger(L, 4);
    objects[index].crop_h = luaL_checkinteger(L, 5);

    return 0;
}

static int L_getPos(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getPos, 1);

    lua_pushnumber(L, objects[index].x);
    lua_pushnumber(L, objects[index].y);

    return 2;
}

static int L_setPos(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setPos, 3);

    objects[index].x = luaL_checkinteger(L, 2);
    objects[index].y = luaL_checkinteger(L, 3);

    return 0;
}

static int L_getSize(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getSize, 1);

    lua_pushnumber(L, objects[index].w);
    lua_pushnumber(L, objects[index].h);

    return 2;
}

static int L_setSize(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setSize, 3);

    objects[index].w = luaL_checkinteger(L, 2);
    objects[index].h = luaL_checkinteger(L, 3);

    return 0;
}

static int L_getSpeed(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getSpeed, 1);

    lua_pushnumber(L, objects[index].speed_x);
    lua_pushnumber(L, objects[index].speed_y);

    return 2;
}

static int L_setSpeed(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setSpeed, 3);

    objects[index].speed_x = luaL_checknumber(L, 2);
    objects[index].speed_y = luaL_checknumber(L, 3);

    return 0;
}

static int L_setUseCamera(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setUseCamera, 2);

    objects[index].use_camera = lua_toboolean(L, 2) != 0;

    return 0;
}

#pragma endregion

static const luaL_Reg L_methods[] = {
    {"createObject",        L_createObject},
    {"clearObjects",        L_clearObjects},
    {"process",             L_process},

    {"getObjectCount",      L_getObjectCount},

    {"setTexture",          L_setTexture},
    {"setTextureCrop",      L_setTextureCrop},
    {"getPos",              L_getPos},
    {"setPos",              L_setPos},
    {"getSize",             L_getSize},
    {"setSize",             L_setSize},
    {"getSpeed",            L_getSpeed},
    {"setSpeed",            L_setSpeed},
    {"setUseCamera",        L_setUseCamera},

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

#undef GAMEOBJECT_REF
#undef GAMEOBJECT_UNREF
#undef CHECK_ARGS_AND_GET_INDEX