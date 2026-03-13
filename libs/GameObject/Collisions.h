#include <stdlib.h>
#include <math.h>

#include "libs/g2d/glib2d.h"
#include "Structs.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

bool checkCollision(Object *a, Object *b);
bool correctPlayerCollision(Object *player, Object *rect, float *dx, float *dy);
void setDefaultRectCollision(Object *rect);