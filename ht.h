// Copyright 2022 Rolea Theodor-Ioan

#ifndef HT_H_
#define HT_H_

#include "utils.h"
#include "ll.h"

typedef struct info
{
	void *key;
	void *value;
} info;

typedef struct ht_t
{
	// Array of linked lists
	ll_t **buckets;
	// Total number of nodes in all buckets combined
	uint size;
	// Number of buckets
	uint hmax;
	// Tells if the key and value are variable (as in they are char *,
	// so they do not have a specific size);
	uint var_key_size;
	uint var_val_size;
	// (Pointer to) Function that calculates the hash
	uint (*hash_function)(void*);
	// (Pointer to) Fuction that compares to keys
	int (*compare_function)(void*, void*);
	// (Pointer to) Function that frees memory realted to the value
	void (*free_function)(void *);
} ht_t;

int
compare_function_strings(void *a, void *b);

uint
hash_function_string(void *a);

ht_t *
ht_create(uint hmax, uint var_key_size, uint var_val_size,
	uint (*hash_function)(void*), int (*compare_function)(void*, void*),
		void (*free_function)(void *));

void
free_buckets(ht_t *ht, void (*free_function)(void *));

void
ht_free(ht_t *ht);

ll_node_t *
find_key(ll_t *bucket, void *key,
	int (*compare_function)(void *, void *), uint *key_pos_bucket);

int
ht_has_key(ht_t *ht, void *key);

void *
ht_get(ht_t *ht, void *key);

void
ht_resize(ht_t *ht, uint key_size, uint value_size,
	void (*ht_put)(ht_t *, void *, uint, void *, uint,
		void (*free_function)(void *)));

void
ht_put(ht_t *ht, void *key, uint key_size, void *value, uint value_size,
	void (*free_function)(void *));

int
ht_remove_entry(ht_t *ht, void *key, void (*free_function)(void *));

#endif  // HT_H_
