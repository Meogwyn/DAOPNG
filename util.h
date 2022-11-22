#ifndef UTIL_H
#define UTIL_H


//prime-generating config
struct rep_t {
	int ngen; //number of generated primes
	int maxp; //largest prime
	int cnt;
	int sqrt;
	struct timespec *time; //time spent generating
};
struct pcfg_t {
	//config file for prime generation
	size_t ubound; //upper bound

	int enumer; //enumerate generated primes
	int report; //print performance report - default 1
	int print; //print prime array

	int sqrt; //search only up to sqrt(2) - default 1
	int prealloc; //predict alloc - default 1 - not yet implemented
};
//dstr_t - dynamically resizeable string
struct dstr_t {
	char *str;
	size_t size;
	size_t alloc;
};
//myopt_t - return value for add_opt 
struct myopt_t {
	char *optstr;
	size_t oslen; //optstr length
	size_t osall; //optstr alloc

	struct option *longopts;
	size_t lolen; //longopts element count
	size_t loall; //longopts array alloc - includes 'NULL' terminator
};

struct rep_t *init_rep();
void grace_exit(int code);
void print_report(struct rep_t *rep);

//pcfg stuff
struct pcfg_t *init_pcfg(void);
void free_pcfg(struct pcfg_t *in);

//these are all cool n' all, but I probably won't need them...
void push_dstr(struct dstr_t *dest, char src); //push char to dstr
struct dstr_t *init_dstr(size_t salloc); //salloc - starting size
void dstr_resizer(struct dstr_t *dest, size_t asize); //resizes if needed: asize - size of addition to alloc
void realloc_dstr(struct dstr_t *dest, size_t nalloc); //nalloc - new alloc value 
void free_dstr(struct dstr_t *dest);
void reset_dstr(struct dstr_t *dest);
void fset_dstr(struct dstr_t *dest, char *fmt, ...);
void set_dstr(struct dstr_t *dest, char *str); 
void fappend_dstr(struct dstr_t *dest, char *fmt, ...);
void cat_dstr(struct dstr_t *dest, struct dstr_t *src); //append src to dest
void append_dstr(struct dstr_t *dest, char *str); //append str to dest
void shrink_dstr(struct dstr_t *dest); //shrink allocated mem to minimum required


struct myopt_t *init_myopt(char *optstr, struct option *longopts);
//init_myopt - obv
//args:
//optstr - if not NULL, starting value of optstr
//longopts - if not NULL, starting value of longopts

void free_myopt(struct myopt_t *dest);
//getopt-related 

void add_opt(struct myopt_t *copts, int sval, int has_arg, char *lname, int *lflag, int lval, char *desc);
//add_opt - specify possible option to be parsed later
//
//NOTE: if an arg is 'optional', that means that one may 'omit' it
//by setting it to NULL (for char *'s) or 0 (for ints). 
//
//args:
//copts - myopt containing the current optstr and longopts
//sval - short option 
//has_arg - has_arg as defined in getopt(3), extended to sval
//lname - (optional - SEE DESCRIPTION AND NOTES) long option name 
//lflag - (optional) long option flag 
//lval - (optional) long option val 
//desc - (optional) description for help page
//
//what it does:
//add_opt modify copts to include an extra item with values corresponding to the args following copts
//After all the add_opt calls are done, the user will be able to glean from copts
//valid and modified optstr and longopts for their getopt_long calls. Additionally,
//add_opt will append desc (if it isn't NULL), followed by a newline, to helpmsg - a global dstr_t 
//containing the help message for the program
//
//while specifying a short form for the option is mandatory, specifying a long form is not.
//If one wishes to add an option which has only a short form, lname is to be set to NULL.
//This causes the function to ignore all the args following lname. 
//
//has_arg has the same meaning for short-form-only options
//as for options with both (as defined in getopt(3)). This alleviates
//the need for the user to interact with optstr to specify whether or not
//the option takes an argument. 
//
//notes:
//* free_myopt should be called once arg parsing is done
//* due to the 'extra meaningful' nature of lname, there is a risk that
//a developer, upon deciding to change the option to have a long form,
//may incorrectly think that doing so entails only setting lname to the
//string literal name of their option, thus neglecting to make sure the 
//previously garbage values in the following args are correct

void handle_args(int argc, char **argv);

#endif
