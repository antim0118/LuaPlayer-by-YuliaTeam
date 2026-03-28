#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Structs.h"

typedef struct
{
    Object *data;           // указатель на данные
    int size;               // объектов в массиве
    int capacity;           // вместимость
} ObjectArray;


void ObjectArrayInit(ObjectArray *arr);
void ObjectArrayClear(ObjectArray *arr, bool clearPersistent);

/// @brief Создаётся объект, плюсует размер в массиве
/// @return index объекта
int ObjectArrayCreate(ObjectArray *arr);

void ObjectRemoveAtIndex(ObjectArray *arr, int index);
