// Copyright 2022 Rolea Theodor-Ioan

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "ll.h"
#include "ht.h"
#include "book.h"
#include "user.h"

int
main(void)
{
	// Creating the hashtables
	ht_t *library = ht_create(HMAX, 1, 0, hash_function_string,
		compare_function_strings, free_book);
	ht_t *users = ht_create(HMAX, 1, 0, hash_function_string,
		compare_function_strings, NULL);
	ht_t *banned_users = ht_create(HMAX, 1, 1, hash_function_string,
		compare_function_strings, NULL);

	// Breaking down a command line into argumentszz
	char line[LINE_SIZE];
	while (fgets(line, LINE_SIZE, stdin)) {
		int len = strlen(line);

		if (line[len - 1] == '\n')
			line[len - 1] = '\0';

		int argc = 0;
		char argv[NR_ARGS][MAX_BOOK_SIZE] = {{'\0'}};

		break_down_line(line, &argc, argv, '"');

		// Executing different commands
		if (!strcmp(argv[0], "ADD_BOOK")) {
			add_book(library, argv[1], atoi(argv[2]));
		} else if (!strcmp(argv[0], "GET_BOOK")) {
			get_book(library, argv[1]);
		} else if (!strcmp(argv[0], "RMV_BOOK")) {
			remove_book(library, argv[1]);
		} else if (!strcmp(argv[0], "ADD_DEF")) {
			// Creating the def_t struct
			def_t def;
			memcpy(def.key, argv[2], MAX_DEF_NAME_SIZE);
			memcpy(def.val, argv[3], MAX_DEF_NAME_SIZE);
			add_def(library, argv[1], &def);
		} else if (!strcmp(argv[0], "GET_DEF")) {
			get_def(library, argv[1], argv[2]);
		} else if (!strcmp(argv[0], "RMV_DEF")) {
			remove_def(library, argv[1], argv[2]);
		} else if (!strcmp(argv[0], "ADD_USER")) {
			add_user(users, banned_users, argv[1]);
		} else if (!strcmp(argv[0], "BORROW")) {
			borrow(library, users, banned_users, argv[1],
				argv[2], atoi(argv[3]));
		} else if (!strcmp(argv[0], "RETURN")) {
			return_func(library, users, banned_users, argv[1],
				argv[2], atoi(argv[3]), atoi(argv[4]));
		} else if (!strcmp(argv[0], "LOST")) {
			lost(library, users, banned_users, argv[1], argv[2]);
		} else if (!strcmp(argv[0], "EXIT")) {
			printf("Books ranking:\n");
			// Checks if there are any books, then prints them if there are
			if (library->size) {
				top_books(library);
			}
			printf("Users ranking:\n");
			// Checks if there are any users, then prints them if there are
			if (users->size) {
				top_users(users);
			}
			// Frees all allocated memory
			ht_free(library);
			free_users(users, banned_users);
			break;
		} else {
			printf("Invalid command. Please try again.\n");
		}
	}

	return 0;
}
