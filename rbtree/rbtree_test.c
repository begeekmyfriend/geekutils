/*
 * Copyright (C) 2015, Leo Ma <begeekmyfriend@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rbtree.h"

#define N 1024 * 1024
// #define RBTREE_DEBUG

int
main(void)
{
        int i, *key;
        struct rbtree tree;
        struct rbnode sentinel, *min, *max, *node;

        key = malloc(N * sizeof(int));
        if (key == NULL) {
                exit(-1);
        }

        rbtree_init(&tree, &sentinel);

        /* Insert test */
        printf("Add %d nodes...\n", N);
        srandom(time(NULL));
        for (i = 0; i < N; i++) {
                struct rbnode *n = malloc(sizeof(*n));
                if (n == NULL) {
                        exit(-1);
                }
                n->key = n->data = key[i] = random();
                rbtree_insert(&tree, n);
        }

        /* Min and max */
        min = rbtree_min(&tree);
        max = rbtree_max(&tree);
        printf("min:0x%08x max:0x%08x\n", min->data, max->data);

        /* ordered traversal */
        printf("ordered traversal...\n");
        for (node = min; node != max; node = rbtree_next(&tree, node)) {
#ifdef RBTREE_DEBUG
                printf("key:0x%08x value:0x%08x\n", node->key, node->data);
#endif
        }

        /* reversed traversal */
        printf("reversed traversal...\n");
        for (node = max; node != min; node = rbtree_prev(&tree, node)) {
#ifdef RBTREE_DEBUG
                printf("key:0x%08x value:0x%08x\n", node->key, node->data);
#endif
        }

        /* Search test */
        printf("Search each node...\n");
        for (i = 0; i < N; i++) {
                struct rbnode *n = rbtree_search(&tree, key[i]);
#ifdef RBTREE_DEBUG
                printf("key:0x%08x value:0x%08x\n", n->key, n->data);
#endif
        }

        /* Remove test */
        printf("Remove all nodes...\n");
        for (i = 0; i < N; i++) {
                struct rbnode *n = rbtree_search(&tree, key[i]);
                rbtree_delete(&tree, n);
                free(n);
        }

        return 0;
}
