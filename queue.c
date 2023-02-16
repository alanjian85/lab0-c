#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head)
        INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list)
        q_release_element(entry);
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }
    list_add(&element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }
    list_add_tail(&element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *element = list_first_entry(head, element_t, list);
    list_del(&element->list);
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *element = list_last_entry(head, element_t, list);
    list_del(&element->list);
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int size = 0;
    struct list_head *node;
    list_for_each (node, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *slow, *fast;
    slow = fast = head->next;
    for (; fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;
    element_t *element = list_entry(slow, element_t, list);
    list_del(&element->list);
    q_release_element(element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    bool dup = false;
    element_t *prev = NULL, *curr, *safe;
    list_for_each_entry_safe (curr, safe, head, list) {
        bool equal = prev && !strcmp(prev->value, curr->value);
        if (equal || dup) {
            list_del(&prev->list);
            q_release_element(prev);
            dup = equal;
        }
        prev = curr;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    struct list_head *prev = head, *a;
    list_for_each (a, head) {
        struct list_head *b = a->next;
        struct list_head *next = b->next;
        if (b == head)
            break;
        prev->next = b;
        b->prev = prev;
        b->next = a;
        a->prev = b;
        a->next = next;
        next->prev = a;
        prev = a;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node, *safe, *temp;
    list_for_each_safe (node, safe, head) {
        temp = node->prev;
        node->prev = node->next;
        node->next = temp;
    }
    temp = head->prev;
    head->prev = head->next;
    head->next = temp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    int size = 0;
    struct list_head *first_head, temp_head;
    first_head = list_entry(head->next, queue_contex_t, chain)->q;
    INIT_LIST_HEAD(&temp_head);
    for (;;) {
        element_t *minimum = NULL;
        queue_contex_t *entry, *min_context;
        list_for_each_entry (entry, head, chain) {
            if (entry->q == NULL || list_empty(entry->q))
                continue;
            element_t *element = list_first_entry(entry->q, element_t, list);
            if (!minimum || strcmp(element->value, minimum->value) < 0) {
                minimum = element;
                min_context = entry;
            }
        }
        if (!minimum)
            break;
        list_move_tail(&minimum->list, &temp_head);
        size++;
        if (min_context->q != first_head && list_empty(min_context->q))
            min_context->q = NULL;
    }
    list_cut_position(first_head, &temp_head, temp_head.prev);
    return size;
}
