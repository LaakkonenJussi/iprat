CC=gcc
CFLAGS=-Wall -std=gnu99 -g -D__NO_ABBR
LDFLAGS=
SOURCES=src/validator.c src/model_calc_functions.c src/math_functions.c src/state_creation.c src/state_container.c src/tests.c
EXEC=validator

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXEC)

csv:
	$(CC) $(CFLAGS) -D__CSV $(LDFLAGS) $(SOURCES) -o $(EXEC)
clean:
	@if [ -f $(EXEC) ] ; then rm $(EXEC) && echo "removed $(EXEC)" ; fi
	
tests:
	make all
	./validator -t 10
	./validator -t 11
	./validator -t 12

