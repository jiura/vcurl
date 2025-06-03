/* 
	Do #define HELPERS_IMPLEMENTATION before include for only one .c file
	For all others, just #include "helpers.h"
*/

#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// For byte-wise operations that use char
typedef char byte;

/* BOOL TYPE START */

typedef uint8_t bool;

#define false 0
#define true 1

/* BOOL TYPE END */

/* DYNAMIC ARRAY START */

typedef struct {
	size_t len;
	size_t cap;
	size_t elem_size;
	void* vals;
} DinArray;

DinArray* dinNew(size_t cap, size_t elem_size);

bool dinAppend(DinArray* arr, void* vals, size_t num_elems);

void dinFree(DinArray* arr);

/* DYNAMIC ARRAY END */


#ifdef HELPERS_IMPLEMENTATION
/* DYNAMIC ARRAY START */
DinArray* dinNew(size_t cap, size_t elem_size) {
	if (!cap) return NULL;

	DinArray* arr = malloc(sizeof(DinArray));
	if (!arr) {
		return NULL;
	}

	arr->len = 0;
	arr->cap = cap;

	arr->vals = malloc(cap * elem_size);
	if (!arr->vals) {
		free(arr);
		return NULL;
	}

	return arr;
}

/* Returns true (1) if it works, false (0) if it doesn't. */
bool dinAppend(DinArray* arr, void* vals, size_t num_elems) {
	size_t og_cap = arr->cap;

	while (arr->len + num_elems > arr->cap) {
		arr->cap *= 2;
	}

	void *tmp = realloc(arr->vals, arr->cap * sizeof(*(arr->vals)));
	if (!tmp) {
		return false;
	}

	arr->vals = tmp;
	
	for (size_t i = 0; i < num_elems; ++i) {
		memcpy(
			(byte*)arr->vals + ((i + arr->len) * arr->elem_size),
			(byte*)vals + (i * arr->elem_size),
			arr->elem_size);
	}
	arr->len += num_elems;

	return true;
}

void dinFree(DinArray* arr) {
	free(arr->vals);
	free(arr);
}
/* DYNAMIC ARRAY END */

#endif // HELPERS_IMPLEMENTATION
#endif // HELPERS_H
