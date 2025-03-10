#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
struct list_head *merge_two_list(struct list_head *left,
                                 struct list_head *right);

struct list_head *merge_recur(struct list_head *head);

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry, *safe = NULL;
    list_for_each_entry_safe(entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_ele = malloc(sizeof(element_t));
    if (!new_ele)
        return false;
    new_ele->value = strdup(s);
    if (!new_ele->value) {
        free(new_ele);
        return false;
    }
    list_add(&new_ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_ele = malloc(sizeof(element_t));
    if (!new_ele)
        return false;
    new_ele->value = strdup(s);
    if (!new_ele->value) {
        free(new_ele);
        return false;
    }
    list_add_tail(&new_ele->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *ele = list_first_entry(head, element_t, list);
    if (sp) {
        memcpy(sp, ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&ele->list);
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *ele = list_last_entry(head, element_t, list);
    if (sp) {
        memcpy(sp, ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&ele->list);
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *next;
    int cnt = 0;

    list_for_each(next, head) {
        cnt++;
    }
    return cnt;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head->next, *fast = head->next->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    element_t *del_ele = list_entry(slow, element_t, list);
    list_del(&del_ele->list);
    q_release_element(del_ele);


    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *curr = head->next;
    const element_t *curr_entry = list_entry(curr, element_t, list);

    while ((&curr_entry->list)->next != head) {
        element_t *next_entry = list_entry(curr->next, element_t, list);
        if (strcmp(curr_entry->value, next_entry->value) == 0) {
            list_del(&next_entry->list);
            q_release_element(next_entry);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node, *safe;

    for (node = (head)->next, safe = node->next;
         node != (head) && safe != (head);
         node = node->next, safe = node->next) {
        list_move(node, safe);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node;
    struct list_head *safe;

    list_for_each_safe(node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head)
        return;

    struct list_head *tmp = head;
    LIST_HEAD(tmp_head);

    struct list_head *node, *safe;
    int cnt = 0;

    list_for_each_safe(node, safe, head) {
        cnt++;
        if (cnt == k) {
            list_cut_position(&tmp_head, tmp, node);
            q_reverse(&tmp_head);
            list_splice_init(&tmp_head, tmp);
        }
        cnt = 0;
        tmp = node->next;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    // disconnect the circular structure
    head->prev->next = NULL;
    head->next = merge_recur(head->next);
    if (!descend) {
        // reconnect the list (prev and circular)
        struct list_head *c = head, *n = head->next;
        while (n) {
            n->prev = c;
            c = n;
            n = n->next;
        }
        c->next = head;
        head->prev = c;
    } else {
        struct list_head *c = head, *n = head->next, *b = head->next->next;
        while (b) {
            n->next = c;
            n->prev = b;
            c = n;
            n = b;
            b = b->next;
        }
        n->next = c;
        n->prev = head;
        head->next = n;
    }
}



/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head)
        return 0;
    struct list_head *curr = head->next;
    element_t const *curr_entry = list_entry(curr, element_t, list);
    while ((&curr_entry->list)->next != head) {
        element_t *next_entry = list_entry(curr->next, element_t, list);
        if (strcmp(curr_entry->value, next_entry->value) < 0) {
            list_del(&next_entry->list);
            q_release_element(next_entry);
        } else {
            curr_entry = next_entry;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head)
        return 0;
    struct list_head *curr = head->next;
    element_t const *curr_entry = list_entry(curr, element_t, list);
    while ((&curr_entry->list)->next != head) {
        element_t *next_entry = list_entry(curr->next, element_t, list);
        if (strcmp(curr_entry->value, next_entry->value) > 0) {
            list_del(&next_entry->list);
            q_release_element(next_entry);
        } else {
            curr_entry = next_entry;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    return 0;
}
struct list_head *merge_two_list(struct list_head *left,
                                 struct list_head *right)
{
    struct list_head head;
    struct list_head *h = &head;
    if (!left && !right) {
        return NULL;
    }
    while (left && right) {
        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) <= 0) {
            h->next = left;
            left = left->next;
            h = h->next;
        } else {
            h->next = right;
            right = right->next;
            h = h->next;
        }
    }
    // after merge, there are still one node still not connect yet
    h->next = left ? left : right;
    return head.next;
}


struct list_head *merge_recur(struct list_head *head)
{
    if (!head->next)
        return head;

    struct list_head *slow = head;
    // split list
    for (const struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next) {
        slow = slow->next;
    }

    struct list_head *mid = slow->next;  // the start node of right part
    slow->next = NULL;

    struct list_head *left = merge_recur(head);
    struct list_head *right = merge_recur(mid);

    return merge_two_list(left, right);
}
