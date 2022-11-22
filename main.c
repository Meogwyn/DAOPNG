#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "util.h"
#include "arr_t.h"
#include "pgen.h"


//TODO: args processing
//TODO: interactive mode / headless mode
//TODO: output file
//TODO: selectable optimizations

//usage:
//./main {options} upper_bound
//
//to generate primes <= upper_bound

struct dstr_t *helpmsg;
struct pcfg_t *cfg;
struct rep_t *report;

int main(int argc, char **argv)
{
	struct arr_t *dest = init_arr(100);

	helpmsg = init_dstr(0);
	cfg = init_pcfg();
	report = init_rep();

	set_dstr(helpmsg, "Dan's Awesome (and Overengineered) Prime Number Generator\n\n");

	handle_args(argc, argv); //changes cfg

	report->algo = cfg->algo;
	switch (cfg->algo) {
		case 1:
			gen_primes(dest, cfg);
			break;
		case 2:
			gen_primes_2(dest, cfg);
			break;
		default:
			printf("algorithm %d doesn't exist!\n", cfg->algo);
			grace_exit(-1);
	}
	if (cfg->print) {
		if (cfg->enumer) {
			printarr_enum(dest);
		}
		else {
			printarr(dest);
		}
	}
	if (cfg->report) {
		print_report(report);
	}
	free_arr(dest);

	return 0;
}
