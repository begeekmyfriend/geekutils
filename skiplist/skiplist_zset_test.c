/*
 * Copyright (C) 2015, Leo Ma <begeekmyfriend@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "t_zset.h"

#define N 2 * 1024 * 1024
// #define SKIPLIST_DEBUG

int
main(void)
{
        int i;
        struct timeval start, end;

	int *key = malloc(N * sizeof(int));
	if (key == NULL) {
		exit(-1);
        }

        zskiplist *zsl = zslCreate();
        if (zsl == NULL) {
                exit(-1);
        }

        printf("Test start!\n");
        printf("Add %d nodes...\n", N);

        /* Insert test */
        srandom(time(NULL));
        gettimeofday(&start, NULL);
        for (i = 0; i < N; i++) {
                key[i] = (int)random();
                zslInsert(zsl, key[i]);
        }
        gettimeofday(&end, NULL);
        printf("time span: %ldms\n", (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);

        /* Search test 1 */
        printf("Now search each node...\n");
        gettimeofday(&start, NULL);
        for (i = 0; i < N; i++) {
                zrangespec range;
                range.min = range.max = key[i];
                range.minex = range.maxex = 0;
                zskiplistNode *zn = zslFirstInRange(zsl, &range);
                if (zn != NULL) {
#ifdef SKIPLIST_DEBUG
                        printf("key:0x%08x value:0x%08x\n", key[i], value);
#endif
                } else {
                        printf("Not found:0x%08x\n", key[i]);
                }
        }
        gettimeofday(&end, NULL);
        printf("time span: %ldms\n", (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);

        /* Search test 2 */
        printf("Now search each node...\n");
        gettimeofday(&start, NULL);
        for (i = 0; i < N; i++) {
                zskiplistNode* zn = zslGetElementByRank(zsl, i + 1);
                if (zn != NULL) {
#ifdef SKIPLIST_DEBUG
                        printf("key:0x%08x value:0x%08x\n", key[i], value);
#endif
                } else {
                        printf("Not found:%d\n", i + 1);
                }
        }
        gettimeofday(&end, NULL);
        printf("time span: %ldms\n", (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);

        /* Delete test */
        printf("Now remove all nodes...\n");
        gettimeofday(&start, NULL);
        for (i = 0; i < N; i++) {
                zslDelete(zsl, key[i]);
        }
        gettimeofday(&end, NULL);
        printf("time span: %ldms\n", (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);

        printf("End of Test.\n");
        zslFree(zsl);

        return 0;  
}
