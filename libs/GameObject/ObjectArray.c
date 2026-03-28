#include "ObjectArray.h"

#define INIT_CAPACITY 100
#define GROW_CAPACITY 100

// мб ошибку как-то поменять хз
#define OBJECTARRAY_ERROR()                             \
    fprintf(stderr, "Memory reallocation failed\n");    \
    exit(1);

#pragma region Static
static void object_array_grow(ObjectArray *arr) {
    int new_capacity = arr->capacity + GROW_CAPACITY;
    Object *new_data = (Object *)realloc(arr->data, new_capacity * sizeof(Object));
    if (!new_data) {
        OBJECTARRAY_ERROR();
    }
    arr->data = new_data;

    memset(arr->data + arr->capacity, 0, (new_capacity - arr->capacity) * sizeof(Object));
    arr->capacity = new_capacity;
}

// очистка всех объектов
static void object_array_clear(ObjectArray *arr) {
    free(arr->data);
    arr->size = 0;
    arr->capacity = INIT_CAPACITY;
    arr->data = (Object *)calloc(arr->capacity, sizeof(Object));
    if (!arr->data) {
        fprintf(stderr, "Memory allocation failed on clear\n");
        exit(1);
    }
}

// очистка всех объектов кроме is_persistent
static void object_array_remove_non_persistent(ObjectArray *arr) {
    int write_idx = 0;
    for (int read_idx = 0; read_idx < arr->size; read_idx++) {
        if (arr->data[read_idx].is_persistent) {
            if (write_idx != read_idx) {
                arr->data[write_idx] = arr->data[read_idx];
            }
            write_idx++;
        }
    }

    for (int i = write_idx; i < arr->size; i++) {
        memset(&arr->data[i], 0, sizeof(Object));
    }
    arr->size = write_idx;
}
#pragma endregion

void ObjectArrayInit(ObjectArray *arr) {
    arr->size = 0;
    arr->capacity = INIT_CAPACITY;
    arr->data = (Object *)calloc(arr->capacity, sizeof(Object));
    if (!arr->data) {
        OBJECTARRAY_ERROR();
    }
}

int ObjectArrayCreate(ObjectArray *arr) {
    if (arr->size >= arr->capacity) {
        object_array_grow(arr);
    }
    int idx = arr->size;
    memset(&arr->data[idx], 0, sizeof(Object));
    arr->size++;
    return idx;
}

void ObjectRemoveAtIndex(ObjectArray *arr, int index) {
    if (index < 0 || index >= arr->size) return;
    for (int i = index; i < arr->size - 1; i++) {
        arr->data[i] = arr->data[i + 1];
    }
    arr->size--;
    memset(&arr->data[arr->size], 0, sizeof(Object));
}

void ObjectArrayClear(ObjectArray *arr, bool clearPersistent) {
    if (clearPersistent) {
        object_array_clear(arr);
    } else {
        object_array_remove_non_persistent(arr);
    }
}

#if false

void ObjectArrayFree(ObjectArray *arr) {
    if (arr->data) {
        free(arr->data);
        arr->data = NULL;
    }
    arr->size = 0;
    arr->capacity = 0;
}

#endif

#undef INIT_CAPACITY
#undef GROW_CAPACITY
#undef OBJECTARRAY_ERROR