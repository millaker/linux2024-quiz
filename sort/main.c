#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "linux_merge.h"
#include "list.h"
#include "td_merge.h"

typedef struct {
    struct list_head list;
    int val;
    int seq;
} element_t;

#define SAMPLES 64000

static void create_sample(struct list_head *head, element_t *space, int samples)
{
#ifdef DEBUG
    printf("Creating sample\n");
#endif
    for (int i = 0; i < samples; i++) {
        element_t *elem = space + i;
        elem->val = rand();
        elem->seq = i;
        list_add_tail(&elem->list, head);
    }
}

static void copy_list(struct list_head *from,
                      struct list_head *to,
                      element_t *space)
{
    if (list_empty(from))
        return;

    element_t *entry;
    list_for_each_entry (entry, from, list) {
        element_t *copy = space++;
        copy->val = entry->val;
        copy->seq = entry->seq;
        list_add_tail(&copy->list, to);
    }
}

int compare(void *priv, const struct list_head *a, const struct list_head *b)
{
    if (a == b)
        return 0;

    int res = list_entry(a, element_t, list)->val -
              list_entry(b, element_t, list)->val;

    if (priv)
        *((int *) priv) += 1;

    return res;
}

bool check_list(struct list_head *head, int count)
{
    if (list_empty(head))
        return 0 == count;

    element_t *entry, *safe;
    size_t ctr = 0;
    list_for_each_entry_safe (entry, safe, head, list) {
        ctr++;
    }
    int unstable = 0;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (entry->list.next != head) {
            if (entry->val > safe->val) {
                fprintf(stderr, "\nERROR: Wrong order\n");
                return false;
            }
            if (entry->val == safe->val && entry->seq > safe->seq)
                unstable++;
        }
    }
    if (unstable) {
        fprintf(stderr, "\nERROR: unstable %d\n", unstable);
        return false;
    }

    if (ctr != SAMPLES) {
        fprintf(stderr, "\nERROR: Inconsistent number of elements: %ld\n", ctr);
        return false;
    }
    return true;
}

typedef void (*test_func_t)(void *priv,
                            struct list_head *head,
                            list_cmp_func_t cmp);

typedef struct {
    char *name;
    test_func_t impl;
} test_t;

int main()
{
    struct list_head sample_head, warmdata_head, testdata_head;
    unsigned long count;
    int nums = SAMPLES;

    FILE *cmps = fopen("cmps_results.txt", "w");

    /* Assume ASLR */
    srand((uintptr_t) &main);

    test_t tests[] = {
        {.name = "list_sort", .impl = list_sort},
        {.name = "td_sort", .impl = td_sort},
        {NULL, NULL},
    };
    test_t *test = tests;

    INIT_LIST_HEAD(&sample_head);

    element_t *samples = malloc(sizeof(*samples) * SAMPLES);
    element_t *warmdata = malloc(sizeof(*warmdata) * SAMPLES);
    element_t *testdata = malloc(sizeof(*testdata) * SAMPLES);

    for (int i = 0; i < 1000; i++) {
        printf("Testing %d\r", i);
        create_sample(&sample_head, samples, nums);
        while (test->impl) {
#ifdef DEBUG
            printf("==== Testing %s ====\n", test->name);
#endif
            /* Warm up */
            INIT_LIST_HEAD(&warmdata_head);
            INIT_LIST_HEAD(&testdata_head);
            copy_list(&sample_head, &testdata_head, testdata);
            copy_list(&sample_head, &warmdata_head, warmdata);
            test->impl(&count, &warmdata_head, compare);

            /* Test */
            count = 0;
            size_t start_time = clock();
            test->impl(&count, &testdata_head, compare);
            size_t end_time = clock();
            fprintf(cmps, "%s:%ld\n", test->name, count);
#ifdef DEBUG
            printf("  Comparisons:    %d\n", count);
            printf("  Time elapsed: %ld\n", end_time - start_time);
            printf("  List is %s\n",
                   check_list(&testdata_head, nums) ? "sorted" : "not sorted");
#endif
            test++;
        }
        test = tests;
    }
    free(samples);
    free(warmdata);
    free(testdata);
    fclose(cmps);
    return 0;
}