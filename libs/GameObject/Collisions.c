#include "Collisions.h"
#include "Object.h"

static bool checkAABBvsAABB(Object *a, Object *b) {
    float ax = a->x + a->cx;
    float ay = a->y + a->cy;
    float bx = b->x + b->cx;
    float by = b->y + b->cy;
    return (ax < bx + b->w) &&
        (ax + a->w > bx) &&
        (ay < by + b->h) &&
        (ay + a->h > by);
}

static bool checkCircleVsAABB(Object *circle, Object *rect) {
    float rx = rect->x + rect->cx;
    float ry = rect->y + rect->cy;
    float closestX = fmaxf(rx, fminf(circle->x, rx + rect->cw));
    float closestY = fmaxf(ry, fminf(circle->y, ry + rect->ch));

    float dx = circle->x - closestX;
    float dy = circle->y - closestY;

    return (dx * dx + dy * dy) < (circle->radius * circle->radius);
}

static bool checkCircleVsCircle(Object *a, Object *b) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float r = a->radius + b->radius;
    return (dx * dx + dy * dy) < (r * r);
}

bool checkCollision(Object *a, Object *b) {
    if (a->collision_shape == COLLISION_RECT) {
        if (b->collision_shape == COLLISION_RECT)
            return checkAABBvsAABB(a, b);
        else if (b->collision_shape == COLLISION_CIRCLE)
            return checkCircleVsAABB(b, a);
    } else if (a->collision_shape == COLLISION_CIRCLE) {
        if (b->collision_shape == COLLISION_RECT)
            return checkCircleVsAABB(a, b);
        else if (b->collision_shape == COLLISION_CIRCLE)
            return checkCircleVsCircle(a, b);
    }
    return false;
}

void SetDefaultRectCollision(Object *rect) {
    if (rect->rotation == 0) {
        rect->cx = -rect->origin_x * rect->w;
        rect->cy = -rect->origin_y * rect->h;
        rect->cw = rect->w;
        rect->ch = rect->h;
    } else {
        float ox = rect->origin_x * rect->w;
        float oy = rect->origin_y * rect->h;
        float corners[4][2] = {
            {-ox,          -oy         },
            {rect->w - ox, -oy         },
            {rect->w - ox,  rect->h - oy},
            {-ox,           rect->h - oy}
        };

        float angle = rect->rotation * M_PI / 180.0f;
        float c = cosf(angle);
        float s = sinf(angle);

        float min_x = INFINITY, max_x = -INFINITY;
        float min_y = INFINITY, max_y = -INFINITY;

        for (int i = 0; i < 4; i++) {
            float x = corners[i][0];
            float y = corners[i][1];

            float rx = x * c - y * s;
            float ry = x * s + y * c;

            if (rx < min_x) min_x = rx;
            if (rx > max_x) max_x = rx;
            if (ry < min_y) min_y = ry;
            if (ry > max_y) max_y = ry;
        }

        rect->cx = min_x;
        rect->cy = min_y;
        rect->cw = max_x - min_x;
        rect->ch = max_y - min_y;
    }
}