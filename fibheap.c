/*
 *  Copyright (c) 2011 Tsitsimpis Ilias
 *  14 January 2011
 *  Version 1.3
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *
 *
 * A Fibonacci Heap library that can be used as
 * minimum or as maximum heap. I have written
 * all functionalities of a priority queue
 * data structure.
 *
 * Based havily on CLRS implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <limits.h>
#include "fibheap.h"

/*
 * If (type>0) then is a max fib heap
 * else is a min fib heap
 */
#define FIB_HEAP_COMPARE(O,N,T) ({ \
		typeof (O) _o = (O); \
		typeof (N) _n = (N); \
		typeof (T) _t = (T); \
		((_t)&&(_n)>(_o)) ? 1 : ( \
		((!(_t))&&(_n)<(_o)) ? 1 : 0 ); })

/* intel assembly */
#define LOG2(X) ({ \
		int _i = 0; \
		__asm__("bsr %1, %0" : "=r" (_i) : "r" ((X))); \
		_i; })

#define CHECK_MALLOC(X) ({ \
		if((X)==NULL) { \
			fprintf(stderr, \
				"fib_heap.c: Error allocating memory\n"); \
			exit(1); \
		}; })

#define CHECK_INPUT(X, M) ({ \
		if(!(X)) {\
			fprintf(stderr, "fib_heap.c: %s\n", (M)); \
			exit(1); \
		}; })


static void
dblink_list_concat(struct fib_heap_node *a, struct fib_heap_node *b)
{
	struct fib_heap_node *temp;
	CHECK_INPUT(a != NULL, "dblink_list_concat: a==NULL");
	CHECK_INPUT(b != NULL, "dblink_list_concat: b==NULL");

	temp = a->right;
	a->right = b->right;
	b->right->left = a;
	b->right = temp;
	temp->left = b;
}

struct fib_heap *
fibheap_init(int max, int t)
{
	struct fib_heap *H = (struct fib_heap *)malloc(sizeof(struct fib_heap));
	CHECK_MALLOC(H);
	CHECK_INPUT(max > 0, "fibheap_init: max has to be positive");

	H->the_one = NULL;
	H->cons_array = (struct fib_heap_node **)
		malloc((LOG2(max)+2) *	sizeof(struct fib_heap_node *));
	CHECK_MALLOC(H->cons_array);
	H->type = (t > 0) ? 1 : 0;
	H->total_nodes = 0;
	H->max_nodes = max;

	return H;
}

struct fib_heap_node *
fibheap_insert(struct fib_heap *H, struct fib_heap_data *d)
{
	struct fib_heap_node *node = (struct fib_heap_node *)
		malloc(sizeof(struct fib_heap_node));
	CHECK_MALLOC(node);
	CHECK_INPUT(H != NULL, "fibheap_insert: H==NULL");
	CHECK_INPUT(H->total_nodes<H->max_nodes,
			"fibheap_insert: Fibonacci Heap Overflow");

	node->data = d;
	node->degree = 0;
	node->marked = 0;
	node->parent = NULL;
	node->child = NULL;

	if((H->the_one) == NULL) {
		node->left = node;
		node->right = node;
		H->the_one = node;
	} else {
		node->left = H->the_one->left;
		node->right = H->the_one;
		H->the_one->left->right = node;
		H->the_one->left = node;
		if(FIB_HEAP_COMPARE(H->the_one->data->key,
					node->data->key, H->type))
			H->the_one = node;
	}
	H->total_nodes++;

	return node;
}

struct fib_heap_data *
fibheap_read(struct fib_heap *H)
{
	CHECK_INPUT(H != NULL, "fibheap_read: H==NULL");
	return (H->the_one->data);
}

struct fib_heap *
fibheap_union(struct fib_heap *a, struct fib_heap *b)
{
	struct fib_heap *temp;
	CHECK_INPUT(a != NULL, "fibheap_union: a==NULL");
	CHECK_INPUT(b != NULL, "fibheap_union: b==NULL");
	CHECK_INPUT(a != b, "fibheap_union: a==b");
	CHECK_INPUT(a->type == b->type,
			"fibheap_union: a is of different type from b");

	if(b->max_nodes > a->max_nodes) {
		temp = a;
		a = b;
		b = temp;
	}

	if((a->the_one) == NULL) {
		a->the_one = b->the_one;
		a->total_nodes = b->total_nodes;
		free(b->cons_array);
		free(b);
	} else if((b->the_one) == NULL) {
		free(b->cons_array);
		free(b);
	} else {
		dblink_list_concat(a->the_one, b->the_one);
		if(FIB_HEAP_COMPARE(a->the_one->data->key,
					b->the_one->data->key, a->type))
			a->the_one = b->the_one;
		a->total_nodes += b->total_nodes;
		free(b->cons_array);
		free(b);
	}
	return a;
}

static void
fibheap_link(struct fib_heap_node *y, struct fib_heap_node *x)
{
	struct fib_heap_node *temp;

	y->left->right = y->right;
	y->right->left = y->left;

	y->parent = x;
	if(x->child == NULL) {
		x->child = y;
		y->left = y;
		y->right = y;
	} else {
		temp = x->child->right;
		x->child->right = y;
		y->left = x->child;
		temp->left = y;
		y->right = temp;
	}

	(x->degree)++;
	y->marked = 0;
}

static void
fibheap_consolidate(struct fib_heap *H)
{
	int D;
	int i, d;
	struct fib_heap_node *x, *w, *y, *temp, *new_one;

	if(H->total_nodes == 0) {
		free(H->the_one);
		H->the_one = NULL;
		return;
	}

	D = LOG2(H->total_nodes);
	for(i=0;i<=D;i++)
		H->cons_array[i] = NULL;

	w = H->the_one->right;
	new_one = w;
	while(w != H->the_one) {
		x = w;
		d = x->degree;
		while((d<=D) && (H->cons_array[d]!=NULL)) {
			y = H->cons_array[d];
			if(FIB_HEAP_COMPARE(x->data->key,
						y->data->key, H->type)) {
				temp = x;
				x = y;
				y = temp;
			}
			if(w == y)
				w = w->left;
			fibheap_link(y, x);
			H->cons_array[d] = NULL;
			d++;
		}
		H->cons_array[d] = x;
		if(!FIB_HEAP_COMPARE(x->data->key, new_one->data->key, H->type))
			new_one = x;
		w = w->right;
	}

	H->the_one->left->right = H->the_one->right;
	H->the_one->right->left = H->the_one->left;
	free(H->the_one);
	H->the_one = new_one;
}

struct fib_heap_data *
fibheap_extract(struct fib_heap *H)
{
	struct fib_heap_data *ret;
	int i;
	CHECK_INPUT(H != NULL, "fibheap_extract: H==NULL");

	if(H->the_one == NULL)
		return NULL;

	ret = H->the_one->data;

	for(i=0;i<H->the_one->degree;i++) {
		H->the_one->child->parent = NULL;
		H->the_one->child = H->the_one->child->right;
	}

	if(H->the_one->child != NULL)
		dblink_list_concat(H->the_one, H->the_one->child);
	(H->total_nodes)--;
	fibheap_consolidate(H);

	return ret;
}

static void
fibheap_cut(struct fib_heap *H,	struct fib_heap_node *x,struct fib_heap_node *y)
{
	if(y->degree == 1) {
		y->child = NULL;
	} else {
		y->child = x->right;
		x->left->right = x->right;
		x->right->left = x->left;
	}
	(y->degree)--;

	x->right = x;
	x->left = x;
	x->parent = NULL;
	x->marked = 0;
	dblink_list_concat(H->the_one, x);
}

/*
 * Yes,  I know this function can easily be re-written
 * as non-recursive. But I think gcc with optimizations
 * can handle a simple tail-recursive function like this.
 * This way is easier and cleaner!
 */
static void
fibheap_casc_cut(struct fib_heap *H, struct fib_heap_node *y)
{
	struct fib_heap_node *z = y->parent;

	if(z == NULL)
		return;

	if(!(y->marked)) {
		y->marked = 1;
		return;
	}

	fibheap_cut(H, y, z);
	return fibheap_casc_cut(H, z);
}

void
fibheap_change(struct fib_heap *H, struct fib_heap_node *node, int new_key)
{
	struct fib_heap_node *y;
	CHECK_INPUT(H != NULL, "fibheap_change: H==NULL");
	CHECK_INPUT(node != NULL, "fibheap_change: node==NULL");
	CHECK_INPUT(FIB_HEAP_COMPARE(node->data->key, new_key, H->type),
			"fibheap_change: not a valid change of key");

	node->data->key = new_key;
	y = node->parent;
	if(y!=NULL&&FIB_HEAP_COMPARE(y->data->key, node->data->key, H->type)) {
		fibheap_cut(H, node, y);
		fibheap_casc_cut(H, y);
	}
	if(FIB_HEAP_COMPARE(H->the_one->data->key, node->data->key, H->type))
		H->the_one = node;
}

void
fibheap_delete(struct fib_heap *H, struct fib_heap_node *node)
{
	CHECK_INPUT(H != NULL, "fibheap_delete: H==NULL");
	CHECK_INPUT(node != NULL, "fibheap_delete: node==NULL");

	if(H->type)
		fibheap_change(H, node, INT_MAX);
	else
		fibheap_change(H, node, INT_MIN);

	free(node->data);
	fibheap_extract(H);
}

static void
fibheap_destroy_rec(struct fib_heap_node *node)
{
	struct fib_heap_node *start = node;

	if(node == NULL)
		return;

	do {
		fibheap_destroy_rec(node->child);
		node = node->right;
		free(node->left->data);
		free(node->left);
	} while(node != start);
}

void
fibheap_destroy(struct fib_heap *H)
{
	CHECK_INPUT(H != NULL, "fibheap_destroy: H==NULL");

	fibheap_destroy_rec(H->the_one);
	free(H->cons_array);
	free(H);
}
