#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "util.h"

extern struct dstr_t *helpmsg;
extern struct pcfg_t *cfg;

struct rep_t *init_rep(void)
{
	struct rep_t *out = malloc(sizeof out);
	
	out->time = malloc(sizeof out->time);

	if ((!out) || (!out->time)) {
		puts("malloc failure in init_rep");
		grace_exit(-1);
	}

	out->ngen = 0;
	out->maxp = 0;
	out->time->tv_sec = 0;
	out->time->tv_nsec = 0;

	return out;
}
void print_report(struct rep_t *in)
{
	if (!in) {
		printf("nothing to in!");
	}
	printf("\n***\n");
	puts("Performance et al:");
	printf("* Time taken: %d seconds, %lu nanoseconds\n", in->time->tv_sec + in->time->tv_nsec / 1000000000, (unsigned long) in->time->tv_nsec % 1000000000);
	printf("* Highest prime: %ld\n", in->maxp);
	printf("* Number of primes: %ld\n", in->ngen);
	printf("\n***\n");
}
void grace_exit(int code)
{
	exit(code);
}
void free_pcfg(struct pcfg_t *in)
{
	free(in);
}
struct pcfg_t *init_pcfg(void)
{
	struct pcfg_t *out = malloc(sizeof(struct pcfg_t));

	if (!out) {
		perror("malloc failed in init_pcfg");
		grace_exit(errno);
	}

	out->ubound = 100;
	out->print = 0;
	out->report = 1;
	out->enumer = 0;

	return out;
}
void push_dstr(struct dstr_t *dest, char src) 
{
	if (!dest->size) {
		dstr_resizer(dest, 2);
		dest->str[0] = src;
		dest->str[1] = '\0';
		dest->size = 2;
		return;
	}
	dstr_resizer(dest, dest->size + 1); //check if an expansion is needed
	dest->str[dest->size - 1] = src;
	dest->str[dest->size] = '\0';
	dest->size++;
}
struct dstr_t *init_dstr(size_t salloc)
{
	struct dstr_t *out = malloc(sizeof(struct dstr_t));
	if (!out) {
		puts("malloc returned NULL (for out in init_dstr)");
		grace_exit(-1);
	}
	out->str = malloc(salloc);
	if (!out) {
		puts("malloc returned NULL (for out->str in init_dstr)");
		grace_exit(-1);
	}
	out->alloc = salloc;
	out->size = 0;
}
void dstr_resizer(struct dstr_t *dest, size_t asize)
{
	if (!dest->alloc) {
		dest->alloc++;
	}
	while (dest->size + asize > dest->alloc) {
		dest->alloc *= 2;
	}
	realloc_dstr(dest, dest->alloc);
}
void realloc_dstr(struct dstr_t *dest, size_t nalloc)
{
	dest->str = realloc(dest->str, nalloc);
	dest->alloc = nalloc;
	if (!dest->str) {
		puts("realloc failed in resize_dstr");
		grace_exit(-1);
	}
}
void free_dstr(struct dstr_t *dest)
{
	free(dest->str);
	free(dest);
}
void reset_dstr(struct dstr_t *dest)
{
	dest->size = 0;
}
void cat_dstr(struct dstr_t *dest, struct dstr_t *src)
{
	dstr_resizer(dest, dest->size + src->size); //will alloc an extra byte most times
	dest->str = strcat(dest->str, src->str);
}
void fset_dstr(struct dstr_t *dest, char *fmt, ...)
{
	//messy but works
	va_list args;
	char *temp;
	
	va_start(args, fmt);
	va_start(args, fmt);
	if (-1 == vasprintf(&temp, fmt, args)) {
		va_end(args);
		puts("vasprintf returned bad code in fset_dstr");
		grace_exit(-1);
	}
	va_end(args);
	set_dstr(dest, temp);
	free(temp);
}
void set_dstr(struct dstr_t *dest, char *str)
{
	dest->size = strlen(str) + 1;
	dstr_resizer(dest, dest->size);
	strcpy(dest->str, str);
	return;
}
void fappend_dstr(struct dstr_t *dest, char *fmt, ...)
{
	//appends formatted string to dest
	//is also messy
	va_list args;
	char *temp;
	
	va_start(args, fmt);
	if (-1 == vasprintf(&temp, fmt, args)) {
		va_end(args);
		puts("vasprintf returned bad code in fset_dstr");
		grace_exit(-1);
	}
	va_end(args);
	append_dstr(dest, temp);
	free(temp);
}
void append_dstr(struct dstr_t *dest, char *str)
{
	if (!dest->size) {
		set_dstr(dest, str);
		return;
	}
	dstr_resizer(dest, dest->size + strlen(str));
	strcat(dest->str, str);
}
void shrink_dstr(struct dstr_t *dest)
{
	realloc_dstr(dest, dest->size);
}
void free_myopt(struct myopt_t *dest)
{
	free(dest->optstr);
	free(dest->longopts);
	free(dest);
}
struct myopt_t *init_myopt(char *optstr, struct option *longopts)
{
	struct myopt_t *out = malloc(sizeof *out);
	size_t locnt = 0; //longopt count

	if (!out) {
		puts("malloc returned NULL (for out in init_myopt)");
		grace_exit(-1);
	}

	if (optstr) {
		out->osall = strlen(optstr) + 1;
		out->optstr = malloc(out->osall);
		strncpy(out->optstr, optstr, out->osall);
		out->oslen = out->osall;
	}
	else {
		out->osall = 1;
		out->optstr = malloc(out->osall);
		out->oslen = 0;
	}

	if (longopts) {
		while (1) {
			if ((longopts[locnt].name == 0) && 
					(longopts[locnt].has_arg == 0) && 
					(longopts[locnt].flag == 0) && 
					(longopts[locnt].val == 0)) {
				locnt++; //include 'NULL' terminator
				break;
			}
			locnt++;
		}
		out->loall = locnt; //includes 'NULL' terminator
		out->longopts = malloc(locnt * sizeof *longopts);
		memcpy(out->longopts, longopts, locnt * sizeof *longopts);
		out->lolen = locnt;
	}
	else {
		out->loall = 1;
		out->longopts = malloc(sizeof *out->longopts);
		out->lolen = 0;
	}

	if (!out->longopts) {
		puts("malloc returned NULL (for out->longopts in init_myopt)");
		grace_exit(-1);
	}
	if (!out->optstr) {
		puts("malloc returned NULL (for out->optstr in init_myopt)");
		grace_exit(-1);
	}
	
	return out;
}
void add_opt(struct myopt_t *copts, int sval, int has_arg, char *lname, int *lflag, int lval, char *desc)
{
	size_t abytes; //how many bytes will be added to optstr
	struct option *temp;
	struct dstr_t *htemp = init_dstr(0); //for appending to help message

	switch (has_arg) {
		case no_argument:
			abytes = 1;
			break;
		case required_argument:
			abytes = 2;
			break;
		case optional_argument:
			abytes = 3;
			break;
		default:
			puts("invalid has_arg in add_opt");
			grace_exit(-1);
			break;
	}

	//resize optstr if needed
	if (copts->osall < copts->oslen + 1 + abytes) {
		copts->osall *= 2; //probably excessive, but eh
		copts->optstr = realloc(copts->optstr, copts->osall);
	}

	//add short option
	copts->optstr[copts->oslen] = sval;
	switch (has_arg) {
		case no_argument:
			break;
		case optional_argument:
			copts->optstr[copts->oslen + 2] = ':';
		case required_argument:
			copts->optstr[copts->oslen + 1] = ':';
			break;
	}
	copts->optstr[copts->oslen + abytes] = '\0';
	copts->oslen += abytes;

	//add help message if needed

	if (!desc)
		return;
	else if (lname) {
		fappend_dstr(helpmsg, "-%c, --%s - %s\n", sval, lname, desc);
	}
	else {
		fappend_dstr(helpmsg, "-%c, - %s\n", sval, desc);
	}
	
	//add long option if needed
	if (!lname)
		return;
	//resize longopts if needed
	if (copts->loall < copts->lolen + 1) {
		copts->loall *= 2;
		copts->longopts = realloc(copts->longopts, copts->loall * sizeof *copts->longopts);
	}

	temp = copts->longopts + copts->lolen;
	temp->name = malloc(strlen(lname) + 1);
	if (!temp->name) {
		puts("malloc returned NULL (for temp->name in add_opt)");
		grace_exit(-1);
	}
	strcpy((char *) temp->name, lname);

	temp->has_arg = has_arg;
	temp->flag = lflag;
	temp->val = lval;

	copts->lolen++;
}
void handle_args(int argc, char **argv)
{
	struct myopt_t *copts = init_myopt(NULL, NULL);
	int ret = 1;

	append_dstr(helpmsg, "USAGE: ./main [options] p\n");
	append_dstr(helpmsg, "./main [options] p ---- generates primes up to and including p\n");

	//ARGUMENT DEFINITIONS:
	add_opt(copts, 'h', no_argument, "help", NULL, 'h', "Print this help");
	add_opt(copts, 'p', no_argument, "print", NULL, 'p', "Print prime array");
	add_opt(copts, 'n', no_argument, "no-report", NULL, 'n', "Do not print performance report");
	add_opt(copts, 'e', no_argument, "enumerate", NULL, 'e', "Enumerate primes in array print-out");

	if (argc == 1) {
		printf("%s", helpmsg->str);
		grace_exit(0);
	}

	//arg parsing
	while (1) {
		ret = getopt_long(argc, argv, copts->optstr, copts->longopts, NULL);
		switch (ret) {
			case -1:
				goto done;
			case '?':
				grace_exit(-1);
			case 'h':
				printf("%s", helpmsg->str);
				break;
			case 'p':
				cfg->print = 1;
				break;
			case 'n':
				cfg->report = 0;
				break;
			case 'e':
				cfg->enumer = 1;
		}
	}

done:
	if (optind >= argc) {
		puts("Expected argument after options");
		grace_exit(-1);
	}
	cfg->ubound = strtol(argv[optind], NULL, 10);
	if (errno) {
		printf("invalid upper bound: %s", optarg); 
		grace_exit(-1);
	}
	free_myopt(copts);
}
