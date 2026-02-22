#include "LUA.h"

extern g2dImage **toG2D(lua_State *L, int index);

clock_t clock_delta;
float delta;

#define GAMEOBJECT_REF(luaIndex, refProp)                           \
if (args >= luaIndex && lua_isfunction(L, luaIndex)) {              \
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

uint32_t getStringHash(const char *str) {
    uint32_t hash = 2166136261u;

    while (*str) {
        hash ^= (uint8_t)*str++;
        hash *= 16777619u;
    }

    return hash;
}

#pragma region BaseObject

typedef struct BaseObject
{
    char *name;
    uint32_t name_hash;

    int ref_oncreate, ref_onupdate, ref_ondraw;
    //4*4=16
} BaseObject;

#define MAX_BASE_OBJECTS 100
static BaseObject baseobjects[MAX_BASE_OBJECTS] = { 0 }; // 16 * 100 = 1600 bytes = 1.56kb

static int getFreeBaseObjectIndex() {
    for (size_t i = 0; i < MAX_BASE_OBJECTS; i++) {
        if (baseobjects[i].name == NULL)
            return i;
    }

    return -1;
}

static int L_createBaseObject(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 1 || args > 4)
        return luaL_error(L, "Objects.createBaseObject(name, [onCreate], [onUpdate], [onDraw]) takes 1-4 arguments");

    int index = getFreeBaseObjectIndex();

    if (index == -1) {
        luaL_error(L, "Objects.createBaseObject() reached max amount of baseobjects");
        return 0;
    }

    const char *text = luaL_checkstring(L, 1);
    baseobjects[index].name = strdup(text);
    baseobjects[index].name_hash = getStringHash(text);

    GAMEOBJECT_REF(2, baseobjects[index].ref_oncreate); // onCreate
    GAMEOBJECT_REF(3, baseobjects[index].ref_onupdate); // onUpdate
    GAMEOBJECT_REF(4, baseobjects[index].ref_ondraw);   // onDraw

    lua_pushnumber(L, index);

    return 1;
}

BaseObject *GetBaseObjectByName(const char *name) {
    if (!name)
        return NULL;

    uint32_t hash = getStringHash(name);

    for (int i = 0; i < MAX_BASE_OBJECTS; i++) {
        if (baseobjects[i].name_hash == hash)
            if (baseobjects[i].name && strcmp(baseobjects[i].name, name) == 0)
                return &baseobjects[i];
    }

    return NULL;
}

static void clearBaseObject(lua_State *L, BaseObject *obj) {
    obj->name = NULL;
    obj->name_hash = 0;
    GAMEOBJECT_UNREF(obj->ref_oncreate);
    GAMEOBJECT_UNREF(obj->ref_onupdate);
    GAMEOBJECT_UNREF(obj->ref_ondraw);
}

#pragma endregion

#pragma region Object
typedef struct Object
{
    BaseObject *base;
    // 4 bytes

    bool isCreated;
    bool isVisible, isEnabled;
    // 3 bytes

    g2dImage *img;
    float x, y, z; //z axis takes from -323768 to 32767 - check setPos
    int w, h;
    int crop_x, crop_y, crop_w, crop_h;
    float speed_x, speed_y;
    float origin_x, origin_y;
    int rotation;
    // 4*15 = 60 bytes

    u32 color;
    int alpha;
    int blending_mode;
    bool use_camera;
    // 4*3 + 1 = 13 bytes

    int ref_state;
    // 4*1 = 4 bytes
} Object;

#define MAX_OBJECTS 2000
static Object objects[MAX_OBJECTS] = { 0 }; // 84 * 2000 = 168k bytes = 164.06kb
static int objects_lastidx = -1;

static int getFreeObjectIndex() {
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
    if (args != 1)
        return luaL_error(L, "Objects.createObject(name) takes 1 argument");

    int index = getFreeObjectIndex();

    if (index == -1) {
        luaL_error(L, "Objects.createObject() reached max amount of objects");
        return 0;
    }

    const char *name = luaL_checkstring(L, 1);
    objects[index].base = GetBaseObjectByName(name);

    //create state table
    if (objects[index].ref_state != -1)
        luaL_unref(L, LUA_REGISTRYINDEX, objects[index].ref_state);
    lua_newtable(L);
    lua_pushstring(L, "id");
    lua_pushnumber(L, index);
    lua_settable(L, -3);
    objects[index].ref_state = luaL_ref(L, LUA_REGISTRYINDEX);

    if (objects_lastidx < index)
        objects_lastidx = index;

    lua_pushnumber(L, index);

    return 1;
}

static void clearObject(lua_State *L, Object *obj) {
    obj->base = NULL;

    obj->isCreated = false;
    obj->isVisible = true;
    obj->isEnabled = true;

    obj->img = NULL;
    obj->x = 0.0f;
    obj->y = 0.0f;
    obj->z = 0.0f;
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

    GAMEOBJECT_UNREF(obj->ref_state);
}



static int L_clearObjects(lua_State *L) {
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        clearObject(L, &objects[i]);
    }
    objects_lastidx = -1;
    clock_delta = clock();

    return 0;
}

#pragma endregion


#pragma region Main

static int L_empty(lua_State *L) {
    luaL_error(L, "empty");

    return 0;
}

void processUpdate(lua_State *L) {
    clock_t clock_delta_now = clock();
    delta = (float)(clock_delta_now - clock_delta) / CLOCKS_PER_SEC;
    clock_delta = clock_delta_now;

    if (objects_lastidx == -1)
        return;

    /* Create */
    for (size_t i = 0; i <= objects_lastidx; i++) {
        Object *obj = &objects[i];
        if (obj->base == NULL || !obj->isEnabled) continue;

        if (!obj->isCreated) {
            if (obj->base->ref_oncreate != -1) {
                lua_rawgeti(L, LUA_REGISTRYINDEX, obj->base->ref_oncreate);
                lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_state);
                lua_pushnumber(L, i);
                lua_call(L, 2, 0); //0=num args   0=num returns //чекнуть тут ещё с error handler
            }
            obj->isCreated = true;
        }
    }

    /* Update */
    for (size_t i = 0; i <= objects_lastidx; i++) {
        Object *obj = &objects[i];
        if (obj->base == NULL || !obj->isEnabled) continue;

        if (obj->base->ref_onupdate != -1) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->base->ref_onupdate);
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_state);
            lua_pushnumber(L, i);
            lua_pushnumber(L, delta);
            lua_call(L, 3, 0); //1=num args   0=num returns //чекнуть тут ещё с error handler
        }

        obj->x += obj->speed_x;
        obj->y += obj->speed_y;
    }
}

void processDraw(lua_State *L) {
    if (objects_lastidx == -1)
        return;

    bool usedCamera = g2dGetUseCamera();
    bool changedTex = true;
    for (size_t i = 0; i <= objects_lastidx; i++) {
        Object *obj = &objects[i];
        if (obj->base == NULL || !obj->isEnabled) continue;

        if (!obj->isVisible) continue;

        if (obj->base->ref_ondraw != -1) {
            // custom draw / onDraw
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->base->ref_ondraw);
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_state);
            lua_pushnumber(L, i);
            lua_pushnumber(L, delta);
            g2dSetUseCamera(obj->use_camera);
            lua_call(L, 3, 0); //1=num args   0=num returns //чекнуть тут ещё с error handler
            changedTex = true;
        } else {
            //default draw
            if (!obj->img) continue;
            if (changedTex) {
                g2dBeginRects(obj->img);
                changedTex = false;
            }
            g2dSetUseCamera(obj->use_camera);
            // g2dSetCoordMode(AlMode);
            // g2dSetTexLinear(linear);
            // g2dSetTexRepeat(repeat);
            g2dSetOriginXY(obj->origin_x, obj->origin_y);
            g2dSetCoordXYZ(obj->x, obj->y, obj->z);
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
            if (i == objects_lastidx || (&objects[i + 1])->img != obj->img) { //last object or texture will be changed
                g2dEnd();
                changedTex = true;
            }
        }
    }
    g2dSetUseCamera(usedCamera);
}

static int L_processAll(lua_State *L) {
    processUpdate(L);
    processDraw(L);

    return 0;
}

static int L_processUpdate(lua_State *L) {
    processUpdate(L);

    return 0;
}

static int L_processDraw(lua_State *L) {
    processDraw(L);

    return 0;
}

#pragma endregion

#pragma region Setters / Getters

#define CHECK_ARGS_AND_GET_INDEX(name, argsNum)                              \
    int args = lua_gettop(L);                                                       \
    if (args != argsNum)                                                            \
        return luaL_error(L, "Objects." #name "() takes " #argsNum " arguments");   \
    int index = luaL_checkinteger(L, 1);

#define CHECK_VARIABLE_ARGS_AND_GET_INDEX(name, argsNumFrom, argsNumTo)                                         \
    int args = lua_gettop(L);                                                                                   \
    if (args < argsNumFrom || args > argsNumTo)                                                                 \
        return luaL_error(L, "Objects." #name "() takes from " #argsNumFrom " to " #argsNumTo " arguments");    \
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
    lua_pushnumber(L, objects[index].z);

    return 3;
}

static int L_setPos(lua_State *L) {
    CHECK_VARIABLE_ARGS_AND_GET_INDEX(setPos, 3, 4);

    objects[index].x = luaL_checknumber(L, 2);
    objects[index].y = luaL_checknumber(L, 3);
    if (args >= 4)
        objects[index].z = 32767 - luaL_checknumber(L, 4);

    return 0;
}

static int L_getOrigin(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getOrigin, 1);

    lua_pushnumber(L, objects[index].origin_x);
    lua_pushnumber(L, objects[index].origin_y);

    return 2;
}

static int L_setOrigin(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setOrigin, 3);

    objects[index].origin_x = luaL_checknumber(L, 2);
    objects[index].origin_y = luaL_checknumber(L, 3);

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
    {"createBaseObject",    L_createBaseObject},
    {"createObject",        L_createObject},
    {"clearObjects",        L_clearObjects},
    {"processAll",          L_processAll},
    {"processUpdate",       L_processUpdate},
    {"processDraw",         L_processDraw},

    {"getObjectCount",      L_getObjectCount},

    {"setTexture",          L_setTexture},
    {"setTextureCrop",      L_setTextureCrop},
    {"getPos",              L_getPos},
    {"setPos",              L_setPos},
    {"getOrigin",           L_getOrigin},
    {"setOrigin",           L_setOrigin},
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