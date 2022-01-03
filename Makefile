CC = gcc
CFLAGS = -g -Wall -Wextra -std=gnu89 -O3
HEADERS = main_header.h
OBJECTS = init_free_functions.o custom_functions.o primary_functions.o secondary_functions.o support_functions.o source.o
EXEC = tema2
IN_FILE = input.in
OUT_FILE = output.out
EXEC_COMMAND = ./$(EXEC) $(IN_FILE) $(OUT_FILE)

.PHONY = default build clean

default: build

build: $(EXEC)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

run: build
	$(EXEC_COMMAND)

run_valgrind: build
	-valgrind --track-origins=yes --leak-check=full -q --log-file=v.out $(EXEC_COMMAND)

clean:
	-rm -f $(OBJECTS)
	-rm -f $(EXEC)