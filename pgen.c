#include <stdio.h>
#include <time.h>
#include <math.h>
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
	fill_arr_upto(dest, cfg->ubound); //fill starting array with consecutive integers
	size_t pbound;
	struct arr_t *temp; //devnote: optimize w/ predicted alloc?
	size_t cur = 0; //for cleaning output array
	struct timespec tempt; 

	if (cfg->sqrt) {
		pbound = floorl(sqrt(cfg->ubound));
		report->sqrt = 1;
	}
	else {
		pbound = cfg->ubound;
		report->sqrt = 0;
	}

	if (cfg->prealloc) {
		//some stuff
		puts("Predicted alloc not supported yet!");
		grace_exit(-1);
	}
	else {
		temp = init_arr(0);
	}

	clock_getres(CLOCK_REALTIME, &tempt);
	printf("using CLOCK_REALTIME, which has resolution %d sec, %ld nsec", tempt.tv_sec, tempt.tv_nsec);
	if (-1 == clock_gettime(CLOCK_REALTIME, &tempt)) {
		puts("clock_gettime failed!");
		grace_exit(-1);
	}

	//stage 1: deal with 2
	temp = add_comps(temp, 2, cfg->ubound); //adds multiples of p into the temp array
	for (int i = 0; i < temp->size - 1; i++) {
		dest->arr[temp->arr[i + 1] - 2] = 0; //cross out multiple of p (except p) 
	}
	reset_arr(&temp); //prepare the temp array for next set of multiples 
	 
	for (unsigned long p = 3;;) {
		//stage 1: use current prime
		temp = add_comps(temp, p, cfg->ubound); //adds multiples of p into the temp array
		for (int i = 0; i < temp->size - 1; i++) {
			dest->arr[temp->arr[i + 1] - 2] = 0; //cross out multiple of p (except p) 
		}
		//TODO: optimize search space
		//stage 2: find next prime
		do {
			report->cnt++;
			p += 2;
			if (p > pbound) {
				goto end;
			}
		} while (!dest->arr[p - 2]); //check for being 'crossed out'
		reset_arr(&temp); //prepare the temp array for next set of multiples 
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

	slice_arr(dest, 0, cur - 1);
	free_arr(temp);
	shrink_arr(dest);

	report->maxp = dest->arr[dest->size - 1]; //for post-generation purposes
	report->ngen = dest->size;

	return dest;
}
unsigned long min(unsigned long a, unsigned long b)
{
	return a < b ? a : b;
}
struct arr_t *gen_primes_2(struct arr_t *dest, struct pcfg_t *cfg)
{
	reset_arr(&dest);
	size_t pbound;
	struct timespec tempt; 

	if (cfg->sqrt) {
		pbound = floorl(sqrt(cfg->ubound));
		report->sqrt = 1;
	}
	else {
		pbound = cfg->ubound;
		report->sqrt = 0;
	}

	if (cfg->prealloc) {
		//some stuff
		puts("Predicted alloc not supported yet!");
		grace_exit(-1);
	}

	clock_getres(CLOCK_REALTIME, &tempt);
	printf("using CLOCK_REALTIME, which has resolution %d sec, %ld nsec", tempt.tv_sec, tempt.tv_nsec);
	if (-1 == clock_gettime(CLOCK_REALTIME, &tempt)) {
		puts("clock_gettime failed!");
		grace_exit(-1);
	}

	//stage 1: deal with 2
	pusharr(&dest, 2);
	 
	for (unsigned long p = 3; p < cfg->ubound; p += 2) {
		//stage 1: use current prime
		for (int i = 0; i < min(dest->size, pbound); i++) {
			if (!(p % dest->arr[i])) {
				goto next;
			}
		}
		pusharr(&dest, p);
next:
	}
end:

	//report stuff
	if (-1 == clock_gettime(CLOCK_REALTIME, report->time)) {
		puts("clock_gettime failed!");
		grace_exit(-1);
	}

	report->time->tv_sec -= tempt.tv_sec;
	report->time->tv_nsec -= tempt.tv_nsec;

	shrink_arr(dest);

	report->maxp = dest->arr[dest->size - 1]; //for post-generation purposes
	report->ngen = dest->size;

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
