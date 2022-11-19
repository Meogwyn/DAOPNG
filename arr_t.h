#ifndef ARR_T_H
#define ARR_T_H
struct arr_t {
	//dynamic unsigned long arrays
	
	//later bignums?
	unsigned long *arr;
	size_t size; //USED bytes
	size_t alloc; //ALLOCATED bytes
};

struct arr_t *init_arr(size_t alloc);
void free_arr(struct arr_t *in);
void reset_arr(struct arr_t **in);
struct arr_t *arrcat(struct arr_t *dest, struct arr_t *src);
void pusharr(struct arr_t **dest, unsigned long in);
void printarr_enum(struct arr_t *in); 
void printarr(struct arr_t *in); 
struct arr_t *slice_arr(struct arr_t *arr, size_t start, size_t end);
struct arr_t *shrink_arr(struct arr_t *arr);

//internal funcs
struct arr_t *realloc_arr(struct arr_t *in, size_t alloc);

#endif
