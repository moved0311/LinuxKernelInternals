#include <stddef.h>

/**
 * container_of() - Calculate address of object that contains address ptr
 * @ptr: pointer to member variable
 * @type: type of the structure containing ptr
 * @member: name of the member variable in struct @type
 *
 * Return: @type pointer of object containing ptr
 */
#ifndef container_of
#define container_of(ptr, type, member)                            \
    __extension__({                                                \
        const __typeof__(((type *) 0)->member) *__pmember = (ptr); \
        (type *) ((char *) __pmember - offsetof(type, member));    \
    })
#endif

/**
 * struct list_head - Head and node of a doubly-linked list
 * @prev: pointer to the previous node in the list
 * @next: pointer to the next node in the list
 */
struct list_head {
    struct list_head *prev, *next;
};

/**
 * LIST_HEAD - Declare list head and initialize it
 * @head: name of the new object
 */
#define LIST_HEAD(head) struct list_head head = {&(head), &(head)}

/**
 * INIT_LIST_HEAD() - Initialize empty list head
 * @head: pointer to list head
 */
static inline void INIT_LIST_HEAD(struct list_head *head)
{
    head->next = head; head->prev = head;
}

/**
 * list_add_tail() - Add a list node to the end of the list
 * @node: pointer to the new node
 * @head: pointer to the head of the list
 */
static inline void list_add_tail(struct list_head *node, struct list_head *head)
{
    struct list_head *prev = head->prev;

    prev->next = node;
    node->next = head;
    node->prev = prev;
    head->prev = node;
}

/**
 * list_del() - Remove a list node from the list
 * @node: pointer to the node
 */
static inline void list_del(struct list_head *node)
{
    struct list_head *next = node->next, *prev = node->prev;
    next->prev = prev; prev->next = next;
}

/**
 * list_empty() - Check if list head has no nodes attached
 * @head: pointer to the head of the list
 */
static inline int list_empty(const struct list_head *head)
{
    return (head->next == head);
}

/**
 * list_is_singular() - Check if list head has exactly one node attached
 * @head: pointer to the head of the list
 */
static inline int list_is_singular(const struct list_head *head)
{
    return (!list_empty(head) && head->prev == head->next);
}

/**
 * list_splice_tail() - Add list nodes from a list to end of another list
 * @list: pointer to the head of the list with the node entries
 * @head: pointer to the head of the list
 */
static inline void list_splice_tail(struct list_head *list,
                                    struct list_head *head)
{
    struct list_head *head_last = head->prev;
    struct list_head *list_first = list->next, *list_last = list->prev;

    if (list_empty(list))
        return;

    head->prev = list_last;
    list_last->next = head;

    list_first->prev = head_last;
    head_last->next = list_first;
}

/**
 * list_cut_position() - Move beginning of a list to another list
 * @head_to: pointer to the head of the list which receives nodes
 * @head_from: pointer to the head of the list
 * @node: pointer to the node in which defines the cutting point
 */
static inline void list_cut_position(struct list_head *head_to,
                                     struct list_head *head_from,
                                     struct list_head *node)
{
    struct list_head *head_from_first = head_from->next;

    if (list_empty(head_from))
        return;

    if (head_from == node) {
        INIT_LIST_HEAD(head_to);
        return;
    }

    head_from->next = node->next;
    head_from->next->prev = head_from;

    head_to->prev = node;
    node->next = head_to;
    head_to->next = head_from_first;
    head_to->next->prev = head_to;
}

/**
 * list_entry() - Calculate address of entry that contains list node
 * @node: pointer to list node
 * @type: type of the entry containing the list node
 * @member: name of the list_head member variable in struct @type
 */
#define list_entry(node, type, member) container_of(node, type, member)

/**
 * list_first_entry() - get first entry of the list
 * @head: pointer to the head of the list
 * @type: type of the entry containing the list node
 * @member: name of the list_head member variable in struct @type
 */
#define list_first_entry(head, type, member) \
    list_entry((head)->next, type, member)

/**
 * list_for_each - iterate over list nodes
 * @node: list_head pointer used as iterator
 * @head: pointer to the head of the list
 */
#define list_for_each(node, head) \
    for (node = (head)->next; node != (head); node = node->next)
