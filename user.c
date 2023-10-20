// Copyright 2022 Rolea Theodor-Ioan

#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "ll.h"
#include "ht.h"
#include "book.h"

// Adds user to the database
void
add_user(ht_t *users, ht_t *banned_users, char name[MAX_DEF_NAME_SIZE])
{
	// Checks if the user is already registered / banned
	if (ht_get(users, name) || ht_get(banned_users, name)) {
		printf("User is already registered.\n");
		return;
	}

	// Init
	user_t user;
	user.days_max = 0;
	user.score = 100;
	// Marks the user as having no book borrowed
	memcpy(user.book_name, INIT_STR, MAX_BOOK_SIZE);
	// Sets the username
	memcpy(user.name, name, MAX_DEF_NAME_SIZE);

	// Puts the user in the database
	ht_put(users, name, strlen(name) + 1, &user, sizeof(user_t),
		users->free_function);
}

/* Marks a book as borrowed, as well as marks a user as having borrowed
 * said book, setting a time limit for its return
 */
void
borrow(ht_t *library, ht_t *users, ht_t *banned_users,
	char user_name[MAX_DEF_NAME_SIZE], char book_name[MAX_BOOK_SIZE],
		int days_max)
{
	// Checks if the user is banned
	if (ht_get(banned_users, user_name)) {
		printf("You are banned from this library.\n");
		return;
	}

	// Gets the user
	user_t *user = ht_get(users, user_name);

	// Checks if the user is registered or already has a book borrowed
	if (!user) {
		printf("You are not registered yet.\n");
		return;
	} else if (strcmp(user->book_name, INIT_STR)) {
		printf("You have already borrowed a book.\n");
		return;
	}

	// Gets the book
	book_t *book = ht_get(library, book_name);

	/* Checks if the book is in the library or if it is already borrowed by
	 * another user
	 */
	if (!book) {
		printf("The book is not in the library.\n");
		return;
	} else if (book->status) {
		printf("The book is borrowed.\n");
		return;
	}

	// Sets the time limit for the book's return
	user->days_max = days_max;
	// Puts the name of the book in the user_t struct
	memcpy(user->book_name, book_name, MAX_BOOK_SIZE);
	// Sets the book's status to borrowed
	book->status = 1;
}

// If the user's score is negative, bans the user
void
check(ht_t *users, ht_t *banned_users, user_t *user)
{
	// Checks the score
	if (user->score < 0) {
		// Puts the user in the banned_users hashtable
		ht_put(banned_users, user->name, strlen(user->name) + 1,
			user->name, strlen(user->name) + 1, banned_users->free_function);
		printf("The user %s has been banned from this library.\n", user->name);
		// Removes the user from the users hashtable (aka the database)
		ht_remove_entry(users, user->name, users->free_function);
	}
}

// Returns a book to the library, adjusting the user's score appropiately
void
return_func(ht_t *library, ht_t *users, ht_t *banned_users,
	char user_name[MAX_DEF_NAME_SIZE], char book_name[MAX_BOOK_SIZE],
		uint days_since, uint rating)
{
	// Checks if the user is banned
	if (ht_get(banned_users, user_name)) {
		printf("You are banned from this library.\n");
		return;
	}

	// Gets the user
	user_t *user = ht_get(users, user_name);

	/* Checks if the user is trying to return a different book than the one
	 * that they borrowed
	 */
	if (strcmp(user->book_name, book_name) ||
		!strcmp(user->book_name, INIT_STR)) {
		printf("You didn't borrow this book.\n");
		return;
	}

	/* Calculates the user's new score:
	 * If the book was returned on time, the score increases by the number of
	 * days that were left until the time limit.
	 * If it was not returned on time, the score decreases by (the number of
	 * days that were over the time limit) * 2.
	 */
	if (days_since > user->days_max)
		user->score -= 2 * (days_since - user->days_max);
	else
		user->score += user->days_max - days_since;

	// Marks the user as having no book
	memcpy(user->book_name, INIT_STR, MAX_BOOK_SIZE);

	// Checks the user's score, banning them if necessary
	check(users, banned_users, user);

	// Gets the book
	book_t *book = ht_get(library, book_name);

	// Sets its status to not borrowed
	book->status = 0;

	/* The book's number of purchases, sum of total ratings, as well as its
	 * average rating change.
	 */
	++(book->purchases);
	book->ratings += rating;
	book->rating_avg = (double)book->ratings / book->purchases;
}

// Removes a book from the library, subtracting 50 from the user's score
void
lost(ht_t *library, ht_t *users, ht_t *banned_users,
	char user_name[MAX_DEF_NAME_SIZE], char book_name[MAX_BOOK_SIZE])
{
	// Checks if the user has been banned
	if (ht_get(banned_users, user_name)) {
		printf("You are banned from this library.\n");
		return;
	}

	// Gets the uer
	user_t *user = ht_get(users, user_name);

	// Checks if the user is registered
	if (!user) {
		printf("You are not registered yet.\n");
		return;
	}

	// Subtracts 50 from the user's score
	user->score -= 50;

	// Marks the user as having no book borrowed
	memcpy(user->book_name, INIT_STR, MAX_BOOK_SIZE);
	// Checks the user's score, banning them if necessary
	check(users, banned_users, user);
	// Removes the book from the library
	remove_book(library, book_name);
}

// Swaps two user_t structs
void
swap_users(user_t *user1, user_t *user2)
{
	user_t aux = *user1;
	*user1 = *user2;
	*user2 = aux;
}

// Prints all users' important information (sorted)
void
top_users(ht_t *users)
{
	// Allocates memory for a vector of user_t structs
	user_t *vector = (user_t *)malloc(users->size * sizeof(user_t));
	DIE(!vector, "vector (users) malloc failed");

	uint cnt = 0;

	// Adds entries from the hashtable in the vector
	ll_node_t *it;
	for (uint i = 0; i < users->hmax; ++i)	{
		if (users->buckets[i]->head) {
			it = users->buckets[i]->head;
			while (it) {
				vector[cnt] = *(user_t *)(((info *)it->data)->value);
				++cnt;
				it = it->next;
			}
		}
	}

	// Sorts the vector based on the given priorities: score, name
	for (uint i = 0; i < cnt - 1; ++i)
		for (uint j = i; j < cnt; ++j) {
			if (vector[i].score < vector[j].score) {
				swap_users(&vector[i], &vector[j]);
			} else if (vector[i].score == vector[j].score) {
				if (strcmp(vector[i].name, vector[j].name) > 0)
					swap_users(&vector[i], &vector[j]);
			}
		}

	// Prints the vector
	for (uint i = 0; i < cnt; ++i) {
		printf("%d. Name:%s Points:%d\n", i + 1, vector[i].name,
			vector[i].score);
	}

	// Frees the vector
	free(vector);
}

// Frees the users and banned_users hashtables
void
free_users(ht_t *users, ht_t *banned_users)
{
	ht_free(users);
	ht_free(banned_users);
}
