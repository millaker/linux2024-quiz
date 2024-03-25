#include "td_merge.h"

static struct list_head *merge_list(void *priv,
                                    struct list_head *l1,
                                    struct list_head *l2,
                                    list_cmp_func_t cmp)
{
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; l1 && l2; *node = (*node)->next) {
        node = (cmp(priv, l1, l2) <= 0) ? &l1 : &l2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    if (l1)
        *ptr = l1;
    else
        *ptr = l2;
    return head;
}

/* Mergesort function for q_sort, head points to the first element of the
 * to be sorted list
 */
static struct list_head *sort_list(void *priv,
                                   struct list_head *head,
                                   list_cmp_func_t cmp)
{
    if (!head || !head->next)
        return head;
    // Find mid node
    struct list_head *mid, *slow = head, *fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    mid = slow->next;
    slow->next = NULL;
    struct list_head *left = sort_list(priv, head, cmp),
                     *right = sort_list(priv, mid, cmp);
    return merge_list(priv, left, right, cmp);
}

/* Sort elements of queue in ascending/descending order */
void td_sort(void *priv, struct list_head *head, list_cmp_func_t cmp)
{
    if (!head || list_empty(head))
        return;
    // Convert list into singly linked list
    head->prev->next = NULL;
    head->next = sort_list(priv, head->next, cmp);
    // Rebuild prev links
    struct list_head *prev = NULL, *curr = head;
    while (curr) {
        curr->prev = prev;
        prev = curr;
        curr = curr->next;
    }
    head->prev = prev;
    prev->next = head;
}