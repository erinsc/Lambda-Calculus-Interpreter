#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef uint_fast32_t num_t;

typedef struct {
  size_t size;
  size_t* values;
} array_t;

typedef struct {
    num_t id;
    num_t* arr;
    size_t size;
} entry_t;

typedef struct {
  size_t size;
  entry_t* entries;
} map_t;

#endif //_TYPES_H
