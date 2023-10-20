// Copyright 2022 Rolea Theodor-Ioan

#ifndef BOOK_H_
#define BOOK_H_

#include "utils.h"
#include "ht.h"

typedef struct book_t
{
	uint ratings;  // the sum of total ratings
	uint purchases;  // the number of purchases
	double rating_avg;  // the average rating
	uint status;  // the book's status: borrowed or not
	char name[MAX_BOOK_SIZE];  // the book's name
	struct ht_t *defs;  // the hashtable of definitions
} book_t;

typedef struct def_t
{
	char key[MAX_DEF_NAME_SIZE];
	char val[MAX_DEF_NAME_SIZE];
} def_t;

void
free_book(void *book);

void
add_book(ht_t *library, char name[MAX_BOOK_SIZE], int num_defs);

void
print_book(book_t *book);

void
get_book(ht_t *library, char name[MAX_BOOK_SIZE]);

void
remove_book(ht_t *library, char name[MAX_BOOK_SIZE]);

void
add_def(ht_t *library, char book_name[MAX_BOOK_SIZE], def_t *def);

void
get_def(ht_t *library, char book_name[MAX_BOOK_SIZE],
	char def_name[MAX_DEF_NAME_SIZE]);

void
remove_def(ht_t *library, char book_name[MAX_BOOK_SIZE],
	char def_name[MAX_DEF_NAME_SIZE]);

void
swap_books(book_t *book1, book_t *book2);

void
top_books(ht_t *library);

#endif  // BOOK_H_
