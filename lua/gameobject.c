#include "gameobject.h"

extern g2dImage **toG2D(lua_State *L, int index);
extern g2dColor *toColor(lua_State *L, int index);
extern g2dColor *pushColor(lua_State *L);

clock_t clock_delta;
float delta;

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
    ObjectArrayClear(&objects, false);
    clock_delta = clock();

    return 0;
}

#pragma endregion


#pragma region Main

static int L_empty(lua_State *L) {
    luaL_error(L, "empty");

    return 0;
}

static void processUpdate(lua_State *L) {
    clock_t clock_delta_now = clock();
    delta = (float)(clock_delta_now - clock_delta) / CLOCKS_PER_SEC;
    clock_delta = clock_delta_now;

    int size = getObjectCount();
    if (size == 0)
        return;

    /* Create */
    for (size_t i = 0; i < size; i++) {
        Object *obj = &objects.data[i];
        if (obj->base == NULL || obj->is_created) continue;

        if (obj->base->ref_oncreate != -1) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->base->ref_oncreate);
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_state);
            lua_pushnumber(L, i);
            lua_call(L, 2, 0); //0=num args   0=num returns
        }
        obj->is_created = true;
    }

    /* Update */
    for (size_t i = 0; i < size; i++) {
        Object *obj = &objects.data[i];
        if (obj->base == NULL || !obj->is_enabled || !obj->is_created) continue;

        if (obj->base->ref_onupdate != -1) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->base->ref_onupdate);
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_state);
            lua_pushnumber(L, i);
            lua_pushnumber(L, delta);
            lua_call(L, 3, 0); //1=num args   0=num returns
        }

        obj->x += obj->speed_x;
        obj->y += obj->speed_y;
    }
}

static void processDraw(lua_State *L) {
    int size = getObjectCount();
    if (size == 0)
        return;

    bool usedCamera = g2dGetUseCamera();
    bool changedTex = true;
    for (size_t i = 0; i < size; i++) {
        Object *obj = &objects.data[i];
        if (obj->base == NULL || !obj->is_visible || !obj->is_created) continue;

        if (obj->base->ref_ondraw != -1) {
            // custom draw / onDraw
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->base->ref_ondraw);
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_state);
            lua_pushnumber(L, i);
            lua_pushnumber(L, delta);
            g2dSetUseCamera(obj->use_camera);
            lua_call(L, 3, 0); //1=num args   0=num returns
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
            g2dSetTexRepeat(obj->use_repeat);
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
            if (i == size - 1 || (&objects.data[i + 1])->img != obj->img) { //last object or texture will be changed
                g2dEnd();
                changedTex = true;
            }
        }
    }
    g2dSetUseCamera(usedCamera);
}

#define PUSHKVSTRING(KEY, VALUE) lua_pushstring(L, KEY); lua_pushstring(L, VALUE); lua_settable(L, -3); 
#define PUSHKVNUMBER(KEY, VALUE) lua_pushstring(L, KEY); lua_pushnumber(L, VALUE); lua_settable(L, -3); 
static void processCollisions(lua_State *L) {
    int size = getObjectCount();
    if (size == 0)
        return;

    for (size_t i = 0; i < size; i++) {
        Object *obj = &objects.data[i];
        if (obj->base == NULL || !obj->is_enabled || !obj->is_created) continue;

        if (!obj->is_solid || obj->base->ref_oncollision == -1) continue;

        bool objIsPlayer = obj->collision_shape == COLLISION_CIRCLE; // is player or npc
        bool preparedData = false;

        int count = 1;
        for (size_t j = 0; j < size; j++) {
            if (i == j)continue;

            Object *other = &objects.data[j];
            if (other->base == NULL || !other->is_enabled || !other->is_solid || other->is_trigger) continue;

            bool doneCorrection = false;
            if (objIsPlayer && other->collision_shape == COLLISION_RECT) {
                float dx = 0.0f, dy = 0.0f;
                if (correctPlayerCollision(obj, other, &dx, &dy)) {
                    obj->x += dx;
                    obj->y += dy;
                    doneCorrection = true;
                }
            }

            if (doneCorrection || checkCollision(obj, other)) {
                if (!preparedData) {
                    preparedData = true;
                    lua_rawgeti(L, LUA_REGISTRYINDEX, obj->base->ref_oncollision);
                    lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_state);
                    lua_pushnumber(L, i);
                    lua_newtable(L);
                }

                lua_pushinteger(L, count);

                lua_pushstring(L, other->base->name);

                lua_settable(L, -3);

                count++;
            }
        }

        if (preparedData) {
            lua_call(L, 3, 0);
        }
    }
}
#undef PUSHKVSTRING
#undef PUSHKVNUMBER

static void drawCollisions(lua_State *L, int alpha) {
    int size = getObjectCount();
    if (size == 0)
        return;

    bool usedCamera = g2dGetUseCamera();
    for (size_t i = 0; i < size; i++) {
        Object *obj = &objects.data[i];
        if (obj->base == NULL || !obj->is_enabled) continue;

        if (!obj->is_solid) continue;

        g2dSetUseCamera(true);
        if (obj->collision_shape == COLLISION_RECT) {
            g2dBeginRects(NULL);
            g2dSetCoordXY(obj->x + obj->cx, obj->y + obj->cy);
            g2dSetScaleWH(obj->cw, obj->ch);
            g2dSetAlpha(alpha);
            g2dSetColor(obj->is_trigger ? 0xFF0080FF : 0xFF0000FF); // trigger ? orange : red
            g2dAdd();
            g2dEnd();
        } else if (obj->collision_shape == COLLISION_CIRCLE) {
            g2dBeginLines(G2D_STRIP);
            g2dSetAlpha(alpha);
            g2dSetColor(0xFF0080FF);

            float x = obj->x;
            float y = obj->y;
            float r = obj->radius;

            g2dSetCoordXY(x - r, y); g2dAdd();
            g2dSetCoordXY(x - r / 1.5f, y - r / 1.5f); g2dAdd();
            g2dSetCoordXY(x, y - r); g2dAdd();
            g2dSetCoordXY(x + r / 1.5f, y - r / 1.5f); g2dAdd();
            g2dSetCoordXY(x + r, y); g2dAdd();
            g2dSetCoordXY(x + r / 1.5f, y + r / 1.5f); g2dAdd();
            g2dSetCoordXY(x, y + r); g2dAdd();
            g2dSetCoordXY(x - r / 1.5f, y + r / 1.5f); g2dAdd();
            g2dSetCoordXY(x - r, y); g2dAdd();

            g2dEnd();
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

static int L_processCollisions(lua_State *L) {
    processCollisions(L);

    return 0;
}

static int L_drawCollisions(lua_State *L) {
    int alpha = setInterval(luaL_checkinteger(L, 1), 0, 255);
    drawCollisions(L, alpha);

    return 0;
}

#pragma endregion

#pragma region Setters / Getters

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

static int L_setVisible(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setVisible, 2);

    objects.data[index].is_visible = lua_toboolean(L, 2) != 0;

    return 0;
}

static int L_getVisible(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getVisible, 1);

    lua_pushboolean(L, objects.data[index].is_visible != 0);

    return 1;
}

static int L_setEnabled(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setEnabled, 2);

    objects.data[index].is_enabled = lua_toboolean(L, 2) != 0;

    return 0;
}

static int L_getEnabled(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getEnabled, 1);

    lua_pushboolean(L, objects.data[index].is_enabled != 0);

    return 1;
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

static int L_getPos(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getPos, 1);

    lua_pushnumber(L, objects.data[index].x);
    lua_pushnumber(L, objects.data[index].y);
    lua_pushnumber(L, objects.data[index].z);

    return 3;
}

static int L_setPos(lua_State *L) {
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

static int L_setCollisionRadius(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(setCollisionRadius, 2);

    objects.data[index].is_solid = true;
    objects.data[index].collision_shape = COLLISION_CIRCLE;
    objects.data[index].radius = luaL_checknumber(L, 2);

    return 0;
}

static int L_getCollisionRadius(lua_State *L) {
    CHECK_ARGS_AND_GET_INDEX(getCollisionRadius, 1);

    lua_pushnumber(L, objects.data[index].radius);

    return 1;
}

#pragma endregion

static const luaL_Reg L_methods[] = {
    {"createBaseObject",    L_createBaseObject},
    {"createObject",        L_createObject},
    {"clearObjects",        L_clearObjects},
    {"processAll",          L_processAll},
    {"processUpdate",       L_processUpdate},
    {"processDraw",         L_processDraw},
    {"processCollisions",   L_processCollisions},
    {"drawCollisions",      L_drawCollisions},

    {"getBaseName",         L_getBaseName},
    {"getObjectCount",      L_getObjectCount},
    {"isObjectExists",      L_isObjectExists},

    {"setVisible",          L_setVisible},
    {"getVisible",          L_getVisible},
    {"setEnabled",          L_setEnabled},
    {"getEnabled",          L_getEnabled},

    {"setTexture",          L_setTexture},
    {"setTextureCrop",      L_setTextureCrop},
    {"getPos",              L_getPos},
    {"setPos",              L_setPos},
    {"getRotation",         L_getRotation},
    {"setRotation",         L_setRotation},
    {"getOrigin",           L_getOrigin},
    {"setOrigin",           L_setOrigin},
    {"getSize",             L_getSize},
    {"setSize",             L_setSize},
    {"getColor",            L_getColor},
    {"setColor",            L_setColor},
    {"getAlpha",            L_getAlpha},
    {"setAlpha",            L_setAlpha},
    {"getSpeed",            L_getSpeed},
    {"setSpeed",            L_setSpeed},
    {"getPersistent",       L_getPersistent},
    {"setPersistent",       L_setPersistent},
    {"setUseCamera",        L_setUseCamera},
    {"setUseRepeat",        L_setUseRepeat},
    {"getState",            L_getState},

    {"setCollisionRect",    L_setCollisionRect},
    {"setCollisionTrigger", L_setCollisionTrigger},
    {"setCollisionRadius",  L_setCollisionRadius},
    {"getCollisionRadius",  L_getCollisionRadius},

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
#undef CHECK_ARGS_AND_GET_INDEX