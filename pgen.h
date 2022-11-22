#ifndef PGEN_H
#define PGEN_H


struct arr_t *add_comps(struct arr_t *dest, unsigned long p, 
		unsigned long ubound); //appends comps from p to dest

unsigned long min(unsigned long a, unsigned long b);
struct arr_t *gen_primes(struct arr_t *dest, struct pcfg_t *cfg);
struct arr_t *gen_primes_2(struct arr_t *dest, struct pcfg_t *cfg);
struct arr_t *fill_arr_upto(struct arr_t *dest, unsigned long upto);

#endif
