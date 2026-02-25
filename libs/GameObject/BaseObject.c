#include "BaseObject.h"

#define MAX_BASE_OBJECTS 100
static BaseObject baseobjects[MAX_BASE_OBJECTS] = { 0 }; // 16 * 100 = 1600 bytes = 1.56kb

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

static uint32_t getStringHash(const char *str) {
    uint32_t hash = 2166136261u;

    while (*str) {
        hash ^= (uint8_t)*str++;
        hash *= 16777619u;
    }

    return hash;
}

static int getFreeBaseObjectIndex() {
    for (size_t i = 0; i < MAX_BASE_OBJECTS; i++) {
        if (baseobjects[i].name == NULL)
            return i;
    }

    return -1;
}

int CreateBaseObject(lua_State *L) {
    int args = lua_gettop(L);
    
    int index = getFreeBaseObjectIndex();

    if (index == -1) {
        return luaL_error(L, "Objects.createBaseObject() reached max amount of baseobjects");
    }

    const char *text = luaL_checkstring(L, 1);
    baseobjects[index].name = strdup(text);
    baseobjects[index].name_hash = getStringHash(text);
    baseobjects[index].ref_oncreate = -1;
    baseobjects[index].ref_onupdate = -1;
    baseobjects[index].ref_ondraw = -1;

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

#undef GAMEOBJECT_REF
#undef GAMEOBJECT_UNREF