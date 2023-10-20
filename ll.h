// Copyright 2022 Rolea Theodor-Ioan

#ifndef LL_H_
#define LL_H_

#include "utils.h"

typedef struct ll_node_t
{
	void* data;
	struct ll_node_t* next;
} ll_node_t;

typedef struct ll_t
{
	ll_node_t* head;
	uint data_size;
	uint size;
} ll_t;

ll_t *
ll_create(uint data_size);

void
ll_add_nth_node(ll_t* list, uint n, const void* new_data);

ll_node_t *
ll_remove_nth_node(ll_t* list, uint n);

void
ll_free(ll_t** pp_list);

#endif  // LL_H_
