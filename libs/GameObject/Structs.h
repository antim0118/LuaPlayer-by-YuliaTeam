#ifndef GAMEOBJECTSTRUCTS_H
#define GAMEOBJECTSTRUCTS_H

#include "libs/g2d/glib2d.h"

typedef enum
{
    COLLISION_NONE,
    COLLISION_RECT,
    COLLISION_CIRCLE
} CollisionShape;

typedef struct BaseObject
{
    char *name;
    uint32_t name_hash;
    //5 bytes

    int ref_oncreate, ref_onupdate, ref_ondraw, ref_oncollision;
    //4*4=16 bytes
} BaseObject;

typedef struct Object
{
    BaseObject *base;
    // 4 bytes

    bool is_created;
    bool is_visible, is_enabled;
    bool is_persistent;
    // 4 bytes

    g2dImage *img;
    float x, y, z;
    int w, h;
    int crop_x, crop_y, crop_w, crop_h;
    float origin_x, origin_y;
    // 4*12 = 48 bytes

    float speed_x, speed_y;
    // 4*2 = 8 bytes

    int rotation;
    u32 color;
    int alpha;
    int blending_mode; //unused
    // 4*4 = 16 bytes

    bool use_camera;
    bool use_repeat;
    // 2 bytes

    /* Collisions */
    bool is_solid, is_trigger;
    CollisionShape collision_shape;
    int cx, cy, cw, ch;
    float radius;
    // 2 + 4*6 = 26 bytes

    /* LUA */
    int ref_state;
    // 4*1 = 4 bytes
} Object;

#endif