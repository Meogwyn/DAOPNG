#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "util.h"
#include "arr_t.h"

extern struct pcfg_t *cfg;

struct arr_t *init_arr(size_t alloc)
{
	struct arr_t *out = malloc(sizeof(struct arr_t));
	
	if (!out) {
		perror("malloc (out) failed in init_arr");
		grace_exit(errno);
	}

	if (alloc) {
		out->arr = malloc(sizeof(unsigned long) * alloc);

		if (!out->arr) {
			perror("malloc (->arr) failed in init_arr");
			grace_exit(errno);
		}
	}
	else {
		out->arr = NULL;
	}
	
	out->alloc = alloc;
	out->size = 0;

	return out;
}
void free_arr(struct arr_t *in)
{
	free(in->arr);
	free(in);
}
void reset_arr(struct arr_t **in)
{
	struct arr_t *temp = *in;
	//make available for future use without free
	temp->size = 0;
}
struct arr_t *arrcat(struct arr_t *dest, struct arr_t *src)
{
	//(really stupid) 'reallocator' to dynamically size arrs
	if (!dest->alloc) {
		dest = realloc_arr(dest, 2);
	}
	while (1) {
		if (dest->size + src->size > dest->alloc) {
			dest = realloc_arr(dest, 2 * dest->alloc);
			continue;
		}
		break;
	}

	memcpy(dest->arr + dest->size, src->arr, sizeof(unsigned long) * src->size);

	dest->size += src->size; 

	return dest;
}
void pusharr(struct arr_t **dest, unsigned long in)
{
	//pushes a single element into dest
	
	//(really stupid) 'reallocator' to dynamically size arrs
	if ((*dest)->size + 1 > (*dest)->alloc) {
		if ((*dest)->alloc) {
			*dest = realloc_arr(*dest, 2 * (*dest)->alloc);
		}
		else {
			*dest = realloc_arr(*dest, 2);
		}
	}
	(*dest)->arr[(*dest)->size] = in;
	(*dest)->size += 1;
}
void printarr_enum(struct arr_t *in)
{
	for (size_t i = 0; i < in->size; i++) {
		printf("%ld: %ld\n", i + 1, in->arr[i]);
	}
}	
void printarr(struct arr_t *in)
{
	for (size_t i = 0; i < in->size; i++) {
		printf("%ld\n", in->arr[i]);
	}
}	
struct arr_t *slice_arr(struct arr_t *arr, size_t start, size_t end)
{
	//Changes array to contain only the elements from start to end (indexes).
	//Shifts the whole shebang to the beginning of the array, thus destroying
	//the elements before start
	//
	//returns pointer to resulting array 
	if (end < start) {
		puts("args error for slicearr - end < start");
		puts("proceeding to exit in shame");
		grace_exit(-1);
	}
	if (end > arr->size - 1) {
		puts("args error for slicearr - end runs over used buffer");
		grace_exit(-1);
	}
	for (size_t i = 0, j = start; i < end - start; i++, j++) {
		arr->arr[i] = arr->arr[j];
	}
	arr->size = end - start + 1;
	return arr;
}
struct arr_t *shrink_arr(struct arr_t *arr)
{
	//shrinks allocated memory to minimum required to store current size
	//it is recommended to run this function after a chain of array-changing
	//operations (those that may decrease the index of the last element
	//of the array that is in use (i.e. arr->size))
	return realloc_arr(arr, arr->size);
}
struct arr_t *realloc_arr(struct arr_t *in, size_t alloc)
{
	//alloc is the number of elements in ->arr, NOT the number of bytes
	in->arr = realloc(in->arr, alloc * sizeof(unsigned long));
//	printf("size: %ld\n", alloc);
	
	if (!in->arr) {
		perror("realloc (out) failed in realloc_arr");
		grace_exit(errno);
	}

	in->alloc = alloc;
	return in;
}
