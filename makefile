CFLAGS += -O3
SRC = main.c pgen.c arr_t.c util.c
HDR = util.h pgen.h arr_t.h
OBJ = main.o pgen.o arr_t.o util.o

LDFLAGS += -lm 

.PHONY: clean run debug

main: $(OBJ) $(HDR)
	$(CC) $(CFLAGS) $(OBJ) -o main $(LDFLAGS)

main.o: main.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@
pgen.o: pgen.c pgen.h util.h arr_t.h
	$(CC) $(CFLAGS) -c $< -o $@
util.o: util.c util.h
	$(CC) $(CFLAGS) -c $< -o $@
arr_t.o: arr_t.c arr_t.h util.h
	$(CC) $(CFLAGS) -c $< -o $@

debug: clean $(SRC) $(HDR) 
	$(CC) -g $(CFLAGS) $(SRC) -o main $(LDFLAGS)

clean:
	-rm -fv *.o main
run: main
	@./main
