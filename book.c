// Copyright 2022 Rolea Theodor-Ioan

#include "book.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "ll.h"
#include "ht.h"

// Frees the hashtable within a book_t struct
void
free_book(void *book)
{
	ht_free(((book_t *)book)->defs);
}

/**
 * @brief Adds a book in the library
 * 
 * @param library the library hashtable
 * @param name the book's name
 * @param num_defs the number of definitions within the book
 */
void
add_book(ht_t *library, char name[MAX_BOOK_SIZE], int num_defs)
{
	// Creates a new book_t struct
	book_t book;
	// Copies its name
	memcpy(book.name, name, MAX_BOOK_SIZE);
	// Init
	book.ratings = book.purchases = book.status = book.rating_avg = 0;

	// Creates the book's hashtable
	book.defs = ht_create(HMAX, 1, 0, hash_function_string,
		compare_function_strings, NULL);

	// Puts the definitions in the book's hashtable
	for (int i = 0; i < num_defs; ++i) {
		def_t def;

		// Reading
		char line[LINE_SIZE];
		fgets(line, LINE_SIZE, stdin);
		int len = strlen(line);

		if (line[len - 1] == '\n')
			line[len - 1] = '\0';

		int argc = 0;
		char argv[NR_ARGS][MAX_BOOK_SIZE] = {{'\0'}};

		break_down_line(line, &argc, argv, '"');

		// Copies the key and the val
		memcpy(def.key, argv[0], MAX_DEF_NAME_SIZE);
		memcpy(def.val, argv[1], MAX_DEF_NAME_SIZE);

		// Puts the definition in the book
		ht_put(book.defs, def.key, strlen(def.key) + 1, &def,
			sizeof(def_t), book.defs->free_function);
	}

	// Puts the the book in the library
	ht_put(library, book.name, strlen(book.name) + 1, &book,
		sizeof(book_t), library->free_function);
}

// Prints a book's important information
void
print_book(book_t *book)
{
	if (!book)
		return;

	printf("Name:%s Rating:%.3lf Purchases:%d\n",
		book->name, book->rating_avg, book->purchases);
}

// Gets a book from the library (searches using its name)
void
get_book(ht_t *library, char name[MAX_BOOK_SIZE])
{
	book_t *book = (book_t *)ht_get(library, name);

	if (!book) {
		printf("The book is not in the library.\n");
		return;
	}

	print_book(book);
}

// Removes a book from the library
void
remove_book(ht_t *library, char name[MAX_BOOK_SIZE])
{
	if (!ht_remove_entry(library, name, free_book))
		printf("The book is not in the library.\n");
}

// Adds a definiton to a given book
void
add_def(ht_t *library, char book_name[MAX_BOOK_SIZE], def_t *def)
{
	// Gets the book
	book_t *book = (book_t *)ht_get(library, book_name);
	if (!book) {
		printf("The book is not in the library.\n");
		return;
	}

	// Adds the new definiton
	ht_put(book->defs, def->key, strlen(def->key) + 1, def,
		sizeof(def_t), book->defs->free_function);
}

/* Gets a definiton from a given book from the library
 * (searches using their name)
 */
void
get_def(ht_t *library, char book_name[MAX_BOOK_SIZE],
	char def_name[MAX_DEF_NAME_SIZE])
{
	// Gets the book
	book_t *book = (book_t *)ht_get(library, book_name);
	if (!book) {
		printf("The book is not in the library.\n");
		return;
	}

	// Gets the definiton
	def_t *def = (def_t *)ht_get(book->defs, def_name);
	if (!def) {
		printf("The definition is not in the book.\n");
		return;
	}

	// Prints the definition
	printf("%s\n", def->val);
}

/* Removes a definiton from a given book from the library
 * (searches using their name)
 */
void
remove_def(ht_t *library, char book_name[MAX_BOOK_SIZE],
	char def_name[MAX_DEF_NAME_SIZE])
{
	// Gets the book
	book_t *book = (book_t *)ht_get(library, book_name);
	if (!book) {
		printf("The book is not in the library.\n");
		return;
	}

	// Removes the definition
	if (!ht_remove_entry(book->defs, def_name, book->defs->free_function))
		printf("The definition is not in the book.\n");
}

// Swaps two book_t structs
void
swap_books(book_t *book1, book_t *book2)
{
	book_t aux = *book1;
	*book1 = *book2;
	*book2 = aux;
}

// Prints all books' important information (sorted)
void
top_books(ht_t *library)
{
	// Allocates memory for a vector of book_t structs
	book_t *vector = (book_t *)malloc(library->size * sizeof(book_t));
	DIE(!vector, "vector (books) malloc failed");

	uint cnt = 0;

	// Adds entries from the hashtable in the vector
	ll_node_t *it;
	for (uint i = 0; i < library->hmax; ++i)	{
		if (library->buckets[i]->head) {
			it = library->buckets[i]->head;
			while (it) {
				vector[cnt] = *(book_t *)(((info *)it->data)->value);
				++cnt;
				it = it->next;
			}
		}
	}

	/* Sorts the vector based on the given priorities: rating,
	 * numner of purchases, name
	 */
	for (uint i = 0; i < cnt - 1; ++i)
		for (uint j = i; j < cnt; ++j) {
			if (vector[i].rating_avg < vector[j].rating_avg) {
				swap_books(&vector[i], &vector[j]);
			} else if (vector[i].rating_avg == vector[j].rating_avg) {
				if (vector[i].purchases < vector[j].purchases) {
					swap_books(&vector[i], &vector[j]);
				} else if (vector[i].purchases == vector[j].purchases) {
					if (strcmp(vector[i].name, vector[j].name) > 0)
						swap_books(&vector[i], &vector[j]);
				}
			}
		}

	// Prints the vector
	for (uint i = 0; i < cnt; ++i) {
		printf("%d. Name:%s Rating:%.3lf Purchases:%d\n",
			i + 1, vector[i].name, vector[i].rating_avg, vector[i].purchases);
	}

	// Frees the vector
	free(vector);
}
