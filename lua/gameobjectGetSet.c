#include "gameobject.h"

extern g2dImage **toG2D(lua_State *L, int index);
extern g2dColor *toColor(lua_State *L, int index);
extern g2dColor *pushColor(lua_State *L);

extern ObjectArray objects;

static void SetSize(int index, int w, int h) {
    objects.data[index].w = w;
    objects.data[index].h = h;
}

#define CHECK_ARGS_AND_GET_INDEX(name, argsNum)                                     \
    int args = lua_gettop(L);                                                       \
    if (args != argsNum)                                                            \
        return luaL_error(L, "Objects." #name "() takes " #argsNum " arguments");   \
    int index = luaL_checkinteger(L, 1);

#define CHECK_VARIABLE_ARGS_AND_GET_INDEX(name, argsNumFrom, argsNumTo)                                         \
    int args = lua_gettop(L);                                                                                   \
    if (args < argsNumFrom || args > argsNumTo)                                                                 \
        return luaL_error(L, "Objects." #name "() takes from " #argsNumFrom " to " #argsNumTo " arguments");    \
    int index = luaL_checkinteger(L, 1);

static int L_getBaseName(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getBaseName, 1);

    lua_pushstring(L, objects.data[index].base->name);

    return 1;
}

static int L_getObjectCount(lua_State *L) {
    lua_pushnumber(L, getObjectCount());

    return 1;
}

static int L_isObjectExists(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1)
        return luaL_error(L, "Objects.isObjectExists() takes 1 arguments");

    int size = getObjectCount();
    if (size == 0) {
        lua_pushboolean(L, false);
        return 1;
    }

    const char *name = luaL_checkstring(L, 1);
    uint32_t name_hash = getStringHash(name);

    for (size_t i = 0; i < size; i++) {
        Object *obj = &objects.data[i];
        if (obj->base == NULL) continue;
        if (obj->base->name_hash == name_hash) {
            if (obj->base->name && strcmp(obj->base->name, name) == 0) {
                lua_pushboolean(L, true);
                return 1;
            }
        }
    }

    lua_pushboolean(L, false);
    return 1;
}

static int L_getVisible(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getVisible, 1);

    lua_pushboolean(L, objects.data[index].is_visible != 0);

    return 1;
}

static int L_setVisible(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setVisible, 2);

    objects.data[index].is_visible = lua_toboolean(L, 2) != 0;

    return 0;
}

static int L_getEnabled(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getEnabled, 1);

    lua_pushboolean(L, objects.data[index].is_enabled != 0);

    return 1;
}

static int L_setEnabled(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setEnabled, 2);

    objects.data[index].is_enabled = lua_toboolean(L, 2) != 0;

    return 0;
}

static int L_setTexture(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setTexture, 2);

    g2dImage *img = *toG2D(L, 2);
    if (!img)
        return luaL_error(L, "Objects.setTexture() can't get the texture");

    objects.data[index].img = img;
    SetSize(index, img->w, img->h);

    return 0;
}

static int L_setTextureCrop(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setTextureCrop, 5);

    objects.data[index].crop_x = luaL_checkinteger(L, 2);
    objects.data[index].crop_y = luaL_checkinteger(L, 3);
    objects.data[index].crop_w = luaL_checkinteger(L, 4);
    objects.data[index].crop_h = luaL_checkinteger(L, 5);

    return 0;
}

static int L_getPosition(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getPos, 1);

    lua_pushnumber(L, objects.data[index].x);
    lua_pushnumber(L, objects.data[index].y);
    lua_pushnumber(L, objects.data[index].z);

    return 3;
}

static int L_setPosition(lua_State *L) {
    CHECK_VARIABLE_ARGS_AND_GET_INDEX(setPos, 3, 4);

    objects.data[index].x = luaL_checknumber(L, 2);
    objects.data[index].y = luaL_checknumber(L, 3);
    if (args >= 4) {
        objects.data[index].z = 32767 - luaL_checknumber(L, 4);
        if (objects.data[index].z < 0 || objects.data[index].z > 65535)
            return luaL_error(L, "Objects.setPos() z axis should be in range of [0-65535] (32767 - z)");
    }

    return 0;
}

static int L_getRotation(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getRotation, 1);

    lua_pushnumber(L, objects.data[index].rotation);

    return 1;
}

static int L_setRotation(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setRotation, 2);

    objects.data[index].rotation = luaL_checkint(L, 2);

    return 0;
}

static int L_getOrigin(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getOrigin, 1);

    lua_pushnumber(L, objects.data[index].origin_x);
    lua_pushnumber(L, objects.data[index].origin_y);

    return 2;
}

static int L_setOrigin(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setOrigin, 3);

    objects.data[index].origin_x = luaL_checknumber(L, 2);
    objects.data[index].origin_y = luaL_checknumber(L, 3);

    return 0;
}

static int L_getSize(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getSize, 1);

    lua_pushinteger(L, objects.data[index].w);
    lua_pushinteger(L, objects.data[index].h);

    return 2;
}

static int L_setSize(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setSize, 3);

    SetSize(index, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));

    return 0;
}

static int L_getColor(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getColor, 1);

    *pushColor(L) = objects.data[index].color;

    return 1;
}

static int L_setColor(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setColor, 2);

    objects.data[index].color = *toColor(L, 2);

    return 0;
}

static int L_getAlpha(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getAlpha, 1);

    lua_pushinteger(L, objects.data[index].alpha);

    return 1;
}

static int L_setAlpha(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setAlpha, 2);

    objects.data[index].alpha = luaL_checkinteger(L, 2);

    return 0;
}

static int L_getSpeed(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getSpeed, 1);

    lua_pushnumber(L, objects.data[index].speed_x);
    lua_pushnumber(L, objects.data[index].speed_y);

    return 2;
}

static int L_setSpeed(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setSpeed, 3);

    objects.data[index].speed_x = luaL_checknumber(L, 2);
    objects.data[index].speed_y = luaL_checknumber(L, 3);

    return 0;
}

static int L_getPersistent(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getPersistent, 1);

    lua_pushboolean(L, objects.data[index].is_persistent);

    return 1;
}

static int L_setPersistent(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setPersistent, 2);

    objects.data[index].is_persistent = lua_toboolean(L, 2) != 0;

    return 0;
}

static int L_setUseCamera(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setUseCamera, 2);

    objects.data[index].use_camera = lua_toboolean(L, 2) != 0;

    return 0;
}

static int L_setUseRepeat(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setUseRepeat, 2);

    objects.data[index].use_repeat = lua_toboolean(L, 2) != 0;

    return 0;
}

static int L_getState(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getState, 1);

    lua_rawgeti(L, LUA_REGISTRYINDEX, objects.data[index].ref_state);

    return 1;
}

static int L_setCollisionRect(lua_State *L) {
    CHECK_VARIABLE_ARGS_AND_GET_INDEX(setCollisionRect, 1, 5);

    objects.data[index].is_solid = true;
    objects.data[index].collision_shape = COLLISION_RECT;

    if (args >= 5) {
        objects.data[index].cx = luaL_checkinteger(L, 2);
        objects.data[index].cy = luaL_checkinteger(L, 3);
        objects.data[index].cw = luaL_checkinteger(L, 4);
        objects.data[index].ch = luaL_checkinteger(L, 5);
    } else {
        setDefaultRectCollision(&objects.data[index]);
    }

    return 0;
}

static int L_setCollisionTrigger(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setCollisionTrigger, 2);

    objects.data[index].is_trigger = lua_toboolean(L, 2) != 0;

    return 0;
}

static int L_getCollisionRadius(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getCollisionRadius, 1);

    lua_pushnumber(L, objects.data[index].radius);

    return 1;
}

static int L_setCollisionRadius(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setCollisionRadius, 2);

    objects.data[index].is_solid = true;
    objects.data[index].collision_shape = COLLISION_CIRCLE;
    objects.data[index].radius = luaL_checknumber(L, 2);

    return 0;
}

#undef CHECK_ARGS_AND_GET_INDEX