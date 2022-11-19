#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>

#include "arr_t.h"
#include "util.h"
#include "pgen.h"

extern struct rep_t *report;

struct arr_t *add_comps(struct arr_t *dest, unsigned long p, unsigned long ubound)
{
	//add multiples of p to dest
	unsigned long temp = p;

	for (unsigned long i = 2; temp <= ubound; i++) {
		pusharr(&dest, temp);
		temp = p * i;
	}

	return dest;
}
struct arr_t *gen_primes(struct arr_t *dest, struct pcfg_t *cfg)
{
	reset_arr(&dest);
	fill_arr_upto(dest, cfg->ubound);
	struct arr_t *temp = init_arr(0); //optimize w/ predicted alloc?
	size_t cur = 0; //for cleaning output array
	struct timespec tempt; 

	if (-1 == clock_gettime(CLOCK_REALTIME, &tempt)) {
		puts("clock_gettime failed!");
		grace_exit(-1);
	}

	for (unsigned long p = 2;;) {
		///stage 1: use current prime
		temp = add_comps(temp, p, cfg->ubound);
		for (int i = 0; i < temp->size - 1; i++) {
			dest->arr[temp->arr[i + 1] - 2] = 0; //cross out comps
		}
		//TODO: optimize search space
		//stage 2: find next prime
		report->maxp = p;
		do {
			p++;
			if (p > cfg->ubound) {
				goto end;
			}
		} while (!dest->arr[p - 2]);
		reset_arr(&temp);
	}
end:
	for (int i = 0; i < dest->size; i++) {
		if (dest->arr[i]) {
			dest->arr[cur] = dest->arr[i];
			cur++;
		}
	}

	//report stuff
	if (-1 == clock_gettime(CLOCK_REALTIME, report->time)) {
		puts("clock_gettime failed!");
		grace_exit(-1);
	}

	report->time->tv_sec -= tempt.tv_sec;
	report->time->tv_nsec -= tempt.tv_nsec;
	report->ngen = cur;

	slice_arr(dest, 0, cur - 1);
	free_arr(temp);
	shrink_arr(dest);

	return dest;
}
struct arr_t *fill_arr_upto(struct arr_t *dest, unsigned long upto)
{
	reset_arr(&dest);
	for (unsigned long i = 0; i <= upto - 2; i++) {
		pusharr(&dest, i + 2); 
	}
	return dest;
}
