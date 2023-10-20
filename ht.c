// Copyright 2022 Rolea Theodor-Ioan

#include "ht.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include "ll.h"
#include "utils.h"

// Compare funtion for strings
int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

// Hashing function for strings
uint
hash_function_string(void *a)
{
	// Credits: http://www.cse.yorku.ca/~oz/hash.html
	unsigned char *puchar_a = (unsigned char*) a;
	int32_t hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;  // hash * 33 + c

	return hash;
}

/**
 * @brief Creates a hashtable
 * 
 * @param hmax number of buckets
 * @param var_key_size signals a key of variable length
 * @param var_val_size signals a value of variable length
 * @param hash_function the hashing function
 * @param compare_function the function used to compare keys
 * @param free_function the function used to free memory allocated for 
 * values (if value is a struct, it may be required to free each field etc.)
 * @return ht_t * 
 */
ht_t *
ht_create(uint hmax, uint var_key_size, uint var_val_size,
	uint (*hash_function)(void*), int (*compare_function)(void*, void*),
		void (*free_function)(void *))
{
	// Allocating memory
	ht_t *ht = (ht_t *)malloc(sizeof(ht_t));
	DIE(!ht, "hashtable malloc failed");
	ht->buckets = (ll_t **)malloc(hmax * sizeof(ll_t *));
	DIE(!ht->buckets, "hashtable->buckets malloc failed");

	// Creating the buckets
	for (uint i = 0; i < hmax; ++i)
		ht->buckets[i] = ll_create(sizeof(info));

	// Init
	ht->size = 0;
	ht->hmax = hmax;
	ht->var_key_size = var_key_size;
	ht->var_val_size = var_val_size;
	ht->hash_function = hash_function;
	ht->compare_function = compare_function;
	ht->free_function = free_function;

	return ht;
}

/**
 * @brief Frees all memory allocated for the buckets of a hashtable
 * 
 * @param ht the hashtable whose buckets are to be freed
 * @param free_function the function used to free memory allocated for 
 * values (if value is a struct, it may be required to free each field etc.)
 */
void
free_buckets(ht_t *ht, void (*free_function)(void *))
{
	ll_node_t *it;
	for (uint i = 0; i < ht->hmax; ++i)	{
		if (ht->buckets[i]->head) {
			it = ht->buckets[i]->head;
			// Goes through all nodes and frees all memory associated to them
			while (it) {
				if (free_function)
					free_function(((info *)it->data)->value);
				free(((info *)it->data)->value);
				free(((info *)it->data)->key);
				it = it->next;
			}
		}
		// Frees the bucket
		ll_free(&ht->buckets[i]);
	}

	// Frees the array of buckets
	free(ht->buckets);
}

// Frees a hashtable
void
ht_free(ht_t *ht)
{
	if (!ht)
		return;

	free_buckets(ht, ht->free_function);
	free(ht);
}

/**
 * @brief Returns a pair of (key, value), as well as its position
 * in the bucket
 * 
 * @param bucket the bucket in which to search
 * @param key a pointer to the key with which to search
 * @param compare_function the function that compares the keys
 * @param key_pos_bucket the position of the pair in the bucket
 * @return ll_node_t * 
 */
ll_node_t *
find_key(ll_t *bucket, void *key,
	int (*compare_function)(void *, void *), uint *key_pos_bucket)
{
	if (!bucket)
		return NULL;

	ll_node_t *it = bucket->head;

	// Searches for the node containing (key, value) in the given bucket
	for (uint i = 0; i < bucket->size; ++i) {
		if (!compare_function(key, ((info *)it->data)->key)) {
			// Gets the node's position in the bucket
			*key_pos_bucket = i;
			return it;
		}
		it = it->next;
	}

	return NULL;
}

// Function returns 1 if it finds a value associated with the given key
int
ht_has_key(ht_t *ht, void *key)
{
	if (!ht)
		return -1;

	// Gets the specific bucket in which to search
	uint index = ht->hash_function(key) % ht->hmax;
	ll_t *bucket = ht->buckets[index];

	uint key_pos_bucket;

	// Searches for the key in the bucket
	ll_node_t *it = find_key(bucket, key, ht->compare_function,
		&key_pos_bucket);

	// If it finds it, returns 1
	if (it)
		return 1;

	// Else, returns 0
	return 0;
}

// Returns a pointer to the value associated with the key
void *
ht_get(ht_t *ht, void *key)
{
	if (!ht)
		return NULL;

	// Gets the specific bucket in which to search
	uint index = ht->hash_function(key) % ht->hmax;
	ll_t *bucket = ht->buckets[index];

	uint key_pos_bucket;

	// Searches for the node containing (key, value) in the given bucket
	ll_node_t *it = find_key(bucket, key, ht->compare_function,
		&key_pos_bucket);

	// If it finds it, it returns a pointer to the value associated to it
	if (it)
		return ((info *)it->data)->value;

	// Else, returns NULL
	return NULL;
}

/**
 * @brief The resize function.
 * 
 * @param ht the hashtable that is to be resized
 * @param key_size the size of a key
 * @param value_size the size of a value
 * @param ht_put a pointer to the put function
 */
void
ht_resize(ht_t *ht, uint key_size, uint value_size,
	void (*ht_put)(ht_t *, void *, uint, void *, uint,
		void (*free_function)(void *)))
{
	// Creates a new hashtable
	ht_t *new_ht = ht_create(2 * ht->hmax, key_size, value_size,
		hash_function_string, compare_function_strings, ht->free_function);

	// Puts all the nodes from the original into the new one
	for (uint i = 0; i < ht->hmax; ++i) {
		ll_t *bucket = ht->buckets[i];
		ll_node_t *it = bucket->head;

		for (uint j = 0; j < bucket->size; ++j) {
			/* If a size is -1, it signals that the key/value is variable,
			 * aka a string (char *)
			 * As such the actual size is strlen(key/value) + 1
			 */
			if (ht->var_key_size) {
				char *key = (char *)((info *)it->data)->key;
				key_size = strlen(key) + 1;
			}
			if (ht->var_val_size) {
				char *val = (char *)((info *)it->data)->value;
				value_size = strlen(val) + 1;
			}
			// Puts the pair in the new hashtable
			ht_put(new_ht, ((info *)it->data)->key, key_size,
				((info *)it->data)->value, value_size, new_ht->free_function);
			it = it->next;
		}
	}

	// Frees the original's buckets
	free_buckets(ht, NULL);

	// Adds the newly created array of buckets to the original
	ht->buckets = new_ht->buckets;

	// Doubles the number of buckets
	ht->hmax *= 2;

	// Frees the new hashtable
	free(new_ht);
}

/**
 * @brief Puts a new pair (key, value) in a hashtable
 * 
 * @param ht the hashtable
 * @param key a pointer to the key
 * @param key_size the key's size
 * @param value a pointer to the value
 * @param value_size the value's size
 * @param free_function the function used to free memory allocated for 
 * values (if value is a struct, it may be required to free each field etc.)
 */
void
ht_put(ht_t *ht, void *key, uint key_size, void *value, uint value_size,
	void (*free_function)(void *))
{
	if (!ht)
		return;

	// Gets the specific bucket in which to search
	uint index = ht->hash_function(key) % ht->hmax;
	ll_t *bucket = ht->buckets[index];

	uint key_pos_bucket;

	// Searches for the node containing (key, value) in the given bucket
	ll_node_t *it = find_key(bucket, key, ht->compare_function,
		&key_pos_bucket);

	info information;

	// If the key is not found, add the pair
	if (!it) {
		information.key = malloc(key_size);
		DIE(!information.key, "information.key malloc failed");
		memcpy(information.key, key, key_size);

		information.value = malloc(value_size);
		DIE(!information.value, "information.value malloc failed");
		memcpy(information.value, value, value_size);

		ll_add_nth_node(bucket, 0, &information);

	/* If it does, frees the value if necessary (value is a struct etc.),
	 * then updates the hashtable.
	 */
	} else {
		if (free_function)
			free_function(((info *)it->data)->value);
		memcpy(((info *)it->data)->value, value, value_size);
	}

	// The hashtable's size ++
	++(ht->size);

	// If necessary, resizes the hashtable
	if ((double )ht->size / ht->hmax > LOAD_FACTOR)
		ht_resize(ht, key_size, value_size, ht_put);
}

/**
 * @brief Removes an entry from the hashtable
 * 
 * @param ht the hashtable
 * @param key a pointer to the key
 * @param free_function the function used to free memory allocated for 
 * values (if value is a struct, it may be required to free each field etc.)
 * @return int (wether)
 */
int
ht_remove_entry(ht_t *ht, void *key, void (*free_function)(void *))
{
	if (!ht)
		return -1;

	// Gets the specific bucket in which to search
	uint index = ht->hash_function(key) % ht->hmax;
	ll_t *bucket = ht->buckets[index];

	uint key_pos_bucket;

	// Searches for the node containing (key, value) in the given bucket
	ll_node_t *it = find_key(bucket, key, ht->compare_function,
		&key_pos_bucket);

	// If the node exists, it removes it and frees all data associated to it
	if (it) {
		it = ll_remove_nth_node(bucket, key_pos_bucket);
		if (free_function)
			free_function(((info *)it->data)->value);
		free(((info *)it->data)->value);
		free(((info *)it->data)->key);
		free(it->data);
		free(it);
		// The hashtable's size --
		--(ht->size);

		return 1;
	}

	return 0;
}
