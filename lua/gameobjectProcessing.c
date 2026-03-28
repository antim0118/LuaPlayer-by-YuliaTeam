#include "gameobject.h"

extern clock_t clock_delta;
static float delta;

extern ObjectArray objects;

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
            g2dSetUseCamera(obj->use_camera);
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->base->ref_ondraw);
            lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_state);
            lua_pushnumber(L, i);
            lua_pushnumber(L, delta);
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

static void drawCollisions(lua_State *L) {
    int size = getObjectCount();
    if (size == 0)
        return;

    int alpha = setInterval(luaL_checkinteger(L, 1), 0, 255);

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

#define CREATE_METHOD_FROM_FUNC(FNAME)  \
static int L_##FNAME(lua_State *L) {    \
    FNAME(L);                           \
    return 0;                           \
}

CREATE_METHOD_FROM_FUNC(processUpdate);
CREATE_METHOD_FROM_FUNC(processDraw);
CREATE_METHOD_FROM_FUNC(processCollisions);
CREATE_METHOD_FROM_FUNC(drawCollisions);

#undef CREATE_METHOD_FROM_FUNC