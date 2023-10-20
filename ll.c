// Copyright 2022 Rolea Theodor-Ioan

#include "ll.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "utils.h"

// Creates a linked list
ll_t *
ll_create(uint data_size)
{
	ll_t* ll;

	// Allocating memory for the list
	ll = malloc(sizeof(*ll));
	DIE(!ll, "linked_list malloc failed");

	// Init
	ll->head = NULL;
	ll->data_size = data_size;
	ll->size = 0;

	return ll;
}

// Adds a node to the list
void
ll_add_nth_node(ll_t* list, uint n, const void* new_data)
{
	ll_node_t *prev, *curr;
	ll_node_t* new_node;

	if (!list) {
		return;
	}

	// n >= list->size => the node is added at the end
	if (n > list->size) {
		n = list->size;
	}

	// Gets to the the position at which the node is going to be added
	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	// Allocating memory for a new node
	new_node = malloc(sizeof(*new_node));
	DIE(!new_node, "new_node malloc failed");
	new_node->data = malloc(list->data_size);
	DIE(!new_node->data, "new_node->data malloc failed");
	memcpy(new_node->data, new_data, list->data_size);

	// Tying the node to the list
	new_node->next = curr;
	if (!prev) {
		// n == 0
		list->head = new_node;
	} else {
		prev->next = new_node;
	}

	// The list's size ++
	list->size++;
}

// Removes a node from a list
ll_node_t *
ll_remove_nth_node(ll_t* list, uint n)
{
	ll_node_t *prev, *curr;

	if (!list || !list->head) {
		return NULL;
	}

	// n >= list->size - 1 => the final node is removed
	if (n > list->size - 1) {
		n = list->size - 1;
	}

	// Gets to the the position from which the node is going to be removed
	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	// Untying the node from the list
	if (!prev) {
		// n == 0
		list->head = curr->next;
	} else {
		prev->next = curr->next;
	}

	// The list's size --
	list->size--;

	return curr;
}

// Frees all memory allocated for a list
void
ll_free(ll_t** pp_list)
{
	ll_node_t* curr;

	if (!pp_list || !*pp_list) {
		return;
	}

	// Removes all nodes and frees the memory
	while ((*pp_list)->size > 0) {
		curr = ll_remove_nth_node(*pp_list, 0);
		free(curr->data);
		free(curr);
	}

	// Frees the list itself
	free(*pp_list);
	*pp_list = NULL;
}
