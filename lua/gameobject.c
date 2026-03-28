#include "gameobject.h"

static clock_t clock_delta;

#define GAMEOBJECT_UNREF(refe)                 \
if (refe != -1) {                              \
    luaL_unref(L, LUA_REGISTRYINDEX, refe);    \
    refe = -1;                                 \
}

#pragma region BaseObject
static int L_createBaseObject(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 1 || args > 5)
        return luaL_error(L, "Objects.createBaseObject(name, [onCreate], [onUpdate], [onDraw], [onCollision]) takes 1-5 arguments");

    return CreateBaseObject(L);
}

static uint32_t getStringHash(const char *str) {
    uint32_t hash = 2166136261u;

    while (*str) {
        hash ^= (uint8_t)*str++;
        hash *= 16777619u;
    }

    return hash;
}
#pragma endregion

#pragma region Object
static ObjectArray objects;

static int getObjectCount() {
    return objects.size;
}

static void resetObjectToDefault(lua_State *L, Object *obj) {
    obj->base = NULL;

    obj->is_created = false;
    obj->is_visible = true;
    obj->is_enabled = true;

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
    obj->origin_x = 0.0f;
    obj->origin_y = 0.0f;

    obj->speed_x = 0.0f;
    obj->speed_y = 0.0f;

    obj->rotation = 0;
    obj->color = 0xFFFFFFFF; //white
    obj->alpha = 255;
    obj->blending_mode = -1;

    obj->use_camera = false;
    obj->use_repeat = false;

    obj->is_solid = false;
    obj->is_trigger = false;
    obj->collision_shape = COLLISION_NONE;
    obj->cx = 0;
    obj->cy = 0;
    obj->cw = 0;
    obj->ch = 0;
    obj->radius = 0.0f;

    GAMEOBJECT_UNREF(obj->ref_state);
}

static int L_createObject(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1)
        return luaL_error(L, "Objects.createObject(name) takes 1 argument");

    const char *name = luaL_checkstring(L, 1);
    BaseObject *base = GetBaseObjectByName(name);
    if (!base) {
        printf("[Gameobject] Object %s wasn't found!\n", name);
        return 0;
    }

    int index = ObjectArrayCreate(&objects);

    resetObjectToDefault(L, &objects.data[index]);

    objects.data[index].base = base;

    //create state table
    if (objects.data[index].ref_state != -1)
        luaL_unref(L, LUA_REGISTRYINDEX, objects.data[index].ref_state);
    lua_newtable(L);
    lua_pushstring(L, "id");
    lua_pushnumber(L, index);
    lua_settable(L, -3);
    objects.data[index].ref_state = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_pushnumber(L, index);

    return 1;
}

static int L_clearObjects(lua_State *L) {
    for (size_t i = 0; i < objects.capacity; i++) {
        resetObjectToDefault(L, &objects.data[i]);
    }

    ObjectArrayClear(&objects, false);
    clock_delta = clock();

    return 0;
}

#pragma endregion

#include "gameobjectProcessing.c"
#include "gameobjectGetSet.c"

#define L_METHOD(M_N) {#M_N, L_##M_N}
#define L_GETSET(GET_M_N, SET_M_N) {#GET_M_N, L_##GET_M_N}, {#SET_M_N, L_##SET_M_N}
static const luaL_Reg L_methods[] = {
    L_METHOD(createBaseObject),
    L_METHOD(createObject),
    L_METHOD(clearObjects),

    L_METHOD(processAll),
    L_METHOD(processUpdate),
    L_METHOD(processDraw),
    L_METHOD(processCollisions),
    L_METHOD(drawCollisions),

    L_METHOD(getBaseName),
    L_METHOD(getObjectCount),
    L_METHOD(isObjectExists),

    L_GETSET(getVisible, setVisible),
    L_GETSET(getEnabled, setEnabled),

    L_METHOD(setTexture),
    L_METHOD(setTextureCrop),
    L_GETSET(getPosition, setPosition),
    L_GETSET(getRotation, setRotation),
    L_GETSET(getOrigin, setOrigin),
    L_GETSET(getSize, setSize),
    L_GETSET(getColor, setColor),
    L_GETSET(getAlpha, setAlpha),
    L_GETSET(getSpeed, setSpeed),
    L_GETSET(getPersistent, setPersistent),
    L_METHOD(setUseCamera),
    L_METHOD(setUseRepeat),
    L_METHOD(getState),

    L_METHOD(setCollisionRect),
    L_METHOD(setCollisionTrigger),
    L_GETSET(setCollisionRadius, getCollisionRadius),

    {0, 0}
};

static const luaL_Reg L_metamethods[] = {
    // {"__gc", L_clearObject},
    {0, 0}
};


int GAMEOBJECT_init(lua_State *L) {
    ObjectArrayInit(&objects);
    L_clearObjects(L);
    // luaL_register(L, "GameObject", L_methods);
    UserdataRegister("GameObject", L_methods, L_metamethods);

    return 0;
}

#undef GAMEOBJECT_UNREF
#undef L_METHOD
#undef L_GETSET