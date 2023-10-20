// Copyright 2022 Rolea Theodor-Ioan

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <errno.h>

// Useful macro for handling errors
#define DIE(assertion, call_description)		\
	do {										\
		if (assertion) {						\
			fprintf(stderr, "(%s, %d): ",		\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}										\
	} while (0)

// Lots of useful defines
#define LINE_SIZE 256
#define MAX_BOOK_SIZE 40
#define MAX_DEF_NAME_SIZE 20
#define HMAX 10
#define uint unsigned int
#define NR_ARGS 100
#define LOAD_FACTOR 1

// The string that marks a user with no book borrowed:
#define INIT_STR "n0_B00k_F04_y0U"

uint
is_delim(char c, char *delim);

char *
my_strtok(char *srcString, char *delim);

char *
my_strtok(char *str, char *delim);

void
break_down_line(char *line, int *argc,
	char argv[NR_ARGS][MAX_BOOK_SIZE], char sep);

#endif  // UTILS_H_
