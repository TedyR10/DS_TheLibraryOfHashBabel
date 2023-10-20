# Copyright 2022 Rolea Theodor-Ioan

# Compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# Defining targets
TARGETS=main

build: $(TARGETS)

main: main.c
		$(CC) $(CFLAGS) -g *.c -o main

pack:
		zip -FSr 313CA_MitranAndreiGabriel_Tema2.zip README Makefile *.c *.h

clean:
		rm -f $(TARGETS)

.PHONY: pack clean