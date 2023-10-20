// Copyright 2022 Rolea Theodor-Ioan

#ifndef USER_H_
#define USER_H_

#include "utils.h"
#include "ht.h"

typedef struct user_t
{
	int score;  // the score, initially 100
	uint days_max;  // the time limit until a book must be returned
	char name[MAX_DEF_NAME_SIZE];  // the username
	char book_name[MAX_BOOK_SIZE];  // the borrowed book's name
} user_t;

void
add_user(ht_t *users, ht_t *banned_users, char name[MAX_DEF_NAME_SIZE]);

void
borrow(ht_t *library, ht_t *users, ht_t *banned_users,
	char user_name[MAX_DEF_NAME_SIZE], char book_name[MAX_BOOK_SIZE],
		int days);

void
check(ht_t *users, ht_t *banned_users, user_t *user);

void
return_func(ht_t *library, ht_t *users, ht_t *banned_users,
	char user_name[MAX_DEF_NAME_SIZE], char book_name[MAX_BOOK_SIZE],
		uint days_since, uint rating);

void
lost(ht_t *library, ht_t *users, ht_t *banned_users,
	char user_name[MAX_DEF_NAME_SIZE], char book_name[MAX_BOOK_SIZE]);

void
swap_users(user_t *user1, user_t *user2);

void
top_users(ht_t *users);

void
free_users(ht_t *users, ht_t *banned_users);

#endif  // USER_H_
