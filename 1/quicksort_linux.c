#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"

typedef struct node {
    int value;
    struct list_head list;
} node;

typedef int (*list_cmp_func_t)(void *,
                               const struct list_head *,
                               const struct list_head *);

/* shuffle array, only work if n < RAND_MAX */
void shuffle(int *array, size_t n)
{
    if (n <= 0)
        return;

    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

struct list_head *list_tail(struct list_head **left)
{
    while ((*left) && (*left)->next)
        left = &((*left)->next);
    return *left;
}

void quicksort(void *priv, struct list_head *head, list_cmp_func_t cmp)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    // Count list length
    int n = 0;
    struct list_head *node;
    list_for_each (node, head) {
        n++;
    }
    // Convert to singly linked list
    head->prev->next = NULL;

    int max_level = 2 * n;
    int i = 0;
    struct list_head *begin[max_level];
    struct list_head *result = NULL, *left = NULL, *right = NULL;

    begin[0] = head->next;
    head->next->prev = head->prev;

    while (i >= 0) {
        struct list_head *L = begin[i], *R = L ? L->prev : L;
        if (L != R) {
            struct list_head *pivot = L;
            struct list_head *p = pivot->next;

            while (p) {
                struct list_head *n = p;
                p = p->next;
                if (cmp(priv, pivot, n) <= 0) {
                    n->next = right;
                    n->prev = right ? right->prev : n;
                    right = n;
                } else {
                    n->next = left;
                    n->prev = left ? left->prev : n;
                    left = n;
                }
            }

            pivot->next = NULL;
            pivot->prev = pivot;
            begin[i] = left;
            begin[i + 1] = pivot;
            begin[i + 2] = right;

            left = right = NULL;
            i += 2;
        } else {
            if (L) {
                L->prev->next = result;
                result = L;
            }
            i--;
        }
    }

    // Rebuild prev link
    struct list_head *curr = result, *prev = head;
    head->next = result;
    do {
        curr->prev = prev;
        prev = curr;
        curr = curr->next;
    } while (curr);
    head->prev = prev;
    prev->next = head;
}

void quicksort_it_tail(void *priv, struct list_head *head, list_cmp_func_t cmp)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    // Count list length
    int n = 0;
    struct list_head *node;
    list_for_each (node, head) {
        n++;
    }
    // Convert to singly linked list
    head->prev->next = NULL;

    int max_level = 2 * n;
    int i = 0;
    struct list_head *begin[max_level];
    struct list_head *result = NULL, *left = NULL, *right = NULL;

    begin[0] = head->next;

    while (i >= 0) {
        struct list_head *L = begin[i], *R = list_tail(&L);
        if (L != R) {
            struct list_head *pivot = L;
            struct list_head *p = pivot->next;

            while (p) {
                struct list_head *n = p;
                p = p->next;
                if (cmp(priv, pivot, n) <= 0) {
                    n->next = right;
                    right = n;
                } else {
                    n->next = left;
                    left = n;
                }
            }

            pivot->next = NULL;
            begin[i] = left;
            begin[i + 1] = pivot;
            begin[i + 2] = right;

            left = right = NULL;
            i += 2;
        } else {
            if (L) {
                list_tail(&L)->next = result;
                result = L;
            }
            i--;
        }
    }

    // Rebuild prev link
    struct list_head *curr = result, *prev = head;
    head->next = result;
    do {
        curr->prev = prev;
        prev = curr;
        curr = curr->next;
    } while (curr);
    head->prev = prev;
    prev->next = head;
}

int cmp(void *priv, struct list_head *a, struct list_head *b)
{
    node *a_entry = list_entry(a, node, list);
    node *b_entry = list_entry(b, node, list);
    if (priv)
        *((int *) priv) += 1;
    return a_entry->value - b_entry->value;
}

int list_is_ordered(struct list_head *head)
{
    if (!head || list_empty(head))
        return -1;
    int prev = -1;
    node *entry;
    list_for_each_entry (entry, head, list) {
        if (entry->value < prev)
            return -1;
    }
    return 0;
}

int main()
{
    // Test

    size_t count = 100000;

    int *test_arr = malloc(sizeof(int) * count);

    for (int i = 0; i < count; ++i)
        test_arr[i] = i;
    shuffle(test_arr, count);

    node head, head1;
    INIT_LIST_HEAD(&head.list);
    INIT_LIST_HEAD(&head1.list);

    while (count--) {
        node *temp = malloc(sizeof(*temp));
        node *temp1 = malloc(sizeof(*temp1));
        temp->value = test_arr[count];
        temp1->value = test_arr[count];
        list_add(&temp->list, &head.list);
        list_add(&temp1->list, &head1.list);
    }


    size_t tbegin = clock();
    quicksort(NULL, &head.list, (list_cmp_func_t) cmp);
    size_t tend = clock();
    size_t result = tend - tbegin;
    printf("New: %ld\n", result);
    tbegin = clock();
    quicksort_it_tail(NULL, &head1.list, (list_cmp_func_t) cmp);
    tend = clock();
    size_t result1 = tend - tbegin;
    printf("Old: %ld\n", result1);
    printf("Improve: %f\n", ((double) result1 - result) / result1);

    assert(!list_is_ordered(&head.list));
    assert(!list_is_ordered(&head1.list));
    node *entry, *s;
    list_for_each_entry_safe (entry, s, &head.list, list) {
        free(entry);
    }
    list_for_each_entry_safe (entry, s, &head1.list, list) {
        free(entry);
    }
    free(test_arr);
    return 0;
}