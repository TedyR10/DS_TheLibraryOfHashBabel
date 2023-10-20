// Copyright 2022 Rolea Theodor-Ioan

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Checks if a character is considered a delimiter
uint
is_delim(char c, char *delim)
{
	while (*delim != '\0') {
		if (c == *delim)
			return 1;

		++delim;
	}

	return 0;
}

// An implementation of strtok
char *
my_strtok(char *src_str, char *delim)
{
	// Start of the next token
	static char *backup_string;

	if (!src_str)
		src_str = backup_string;

	if (!src_str)
		return NULL;

	// Handles the beginning of the string containing delimiters
	while (1) {
		if (is_delim(*src_str, delim)) {
			++src_str;
			continue;
		}

	// The end of the string has been reached
		if (*src_str == '\0') {
			return NULL;
		}

		break;
	}

	char *ret = src_str;

	while (1) {
		if (*src_str == '\0') {
			/* The end of the input string and the next 
			 * execution returns NULL
			 */
			backup_string = src_str;

			return ret;
		}

		if (is_delim(*src_str, delim)) {
			*src_str = '\0';

			backup_string = src_str + 1;

			return ret;
		}

		++src_str;
	}
}

/**
 * @brief Breaks down a line into arguments
 * 
 * @param line the command line
 * @param argc the number of arguments
 * @param argv the vector of arguments
 * @param sep a separator sep is used to signal a multi-word argument
 */
void
break_down_line(char *line, int *argc,
	char argv[NR_ARGS][MAX_BOOK_SIZE], char sep)
{
	// The first token
	char *tok = my_strtok(line, "\n ");

	// Goes through all tokens
	while (tok) {
		// The separator sep is detected at the beginning of a token
		if (tok && tok[0] == sep) {
			// sep is deleted
			memmove(tok, tok + 1, strlen(tok) - 1);
			tok[strlen(tok) - 1] = '\0';
			/* Until sep is found at the end of a token, appends the
			 * following tokens to the zoriginal one, in which sep was detected
			 */
			while (tok[strlen(tok) - 1] != '"') {
				strncat(argv[(*argc)], tok, strlen(tok) + 1);
				strncat(argv[(*argc)], " ", strlen(" ") + 1);
				tok = my_strtok(NULL, "\n ");
			}
			strncat(argv[(*argc)], tok, strlen(tok) - 1);

			// The argument count ++
			++(*argc);

			// Gets the next token
			tok = my_strtok(NULL, "\n ");
			if (!tok) {
				return;
			}
		}
		// Copies the token into the vector of arguments
		memcpy(argv[(*argc)], tok, MAX_DEF_NAME_SIZE);
		// The argument count ++
		++(*argc);
		// Gets the next token
		tok = my_strtok(NULL, "\n ");
	}
}
