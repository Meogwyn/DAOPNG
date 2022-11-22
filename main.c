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
	struct arr_t *dest = init_arr(1000);

	helpmsg = init_dstr(0);
	cfg = init_pcfg();
	report = init_rep();

	set_dstr(helpmsg, "Dan's Awesome (and Overengineered) Prime Number Generator\n\n");

	handle_args(argc, argv); //changes cfg

	gen_primes(dest, cfg);
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
