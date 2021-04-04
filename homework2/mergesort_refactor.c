#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef struct __element {
    char *value;
    struct list_head list;
} list_ele_t;


static list_ele_t *head_new(){
  list_ele_t *head = malloc(sizeof(list_ele_t));
  if(!head) return NULL;
  head->value = NULL;
  INIT_LIST_HEAD(&head->list);
  return head;
}

bool insert_list(list_ele_t *head, char *s){
  list_ele_t *newh = malloc(sizeof(list_ele_t));
  if(!newh)
    return false;

  char *new_value = strdup(s);
  if(!new_value){
    free(newh);
    return false;
  }

  newh->value = new_value;
  list_add_tail(&newh->list, &head->list);
  return true;
}

static list_ele_t *get_middle(struct list_head *list)
{
    struct list_head *fast = list->next, *slow;
    list_for_each (slow, list) {
        if (fast->next == list || fast->next->next == list)
            break;
        fast = fast->next->next;
    }
    return list_entry(slow, list_ele_t, list);
}

static void list_merge(struct list_head *lhs,
                       struct list_head *rhs,
                       struct list_head *head)
{
    INIT_LIST_HEAD(head);
    if (list_empty(lhs)) {
        list_splice_tail(lhs, head);
        return;
    }
    if (list_empty(rhs)) {
        list_splice_tail(rhs, head);
        return;
    }

    while (!list_empty(lhs) && !list_empty(rhs)) {
        char *lv = list_entry(lhs->next, list_ele_t, list)->value;
        char *rv = list_entry(rhs->next, list_ele_t, list)->value;
        struct list_head *tmp = strcmp(lv, rv) <= 0 ? lhs->next : rhs->next;
        list_del(tmp);
        list_add_tail(tmp, head);
    }
    list_splice_tail(list_empty(lhs) ? rhs : lhs, head);
}


void list_merge_sort(list_ele_t *head){
  if(list_is_singular(&head->list))
      return;

  list_ele_t left;
  struct list_head sorted;
  INIT_LIST_HEAD(&left.list);
  list_cut_position(&left.list, &head->list, &get_middle(&head->list)->list);
  list_merge_sort(&left);
  list_merge_sort(head);
  list_merge(&left.list, &head->list, &sorted);
  INIT_LIST_HEAD(&head->list); //?
  list_splice_tail(&sorted, &head->list); //?
}

void list_show(struct list_head *list){
  struct list_head *node;
  list_for_each(node, list){
    printf("%s", list_entry(node, list_ele_t, list)->value);
  }
}

static void list_free(struct list_head *list)
{
    if (!list) return;

    struct list_head *current = list;
    while (current != list) {
        struct list_head *tmp = current;
        current = current->next;
        free(list_entry(tmp, list_ele_t, list)->value);
        free(list_entry(tmp, list_ele_t, list));
    }
    free(list_entry(current, list_ele_t, list)->value);
    free(list_entry(current, list_ele_t, list));
}

int main(void)
{
    FILE *fp = fopen("input.txt", "r");
    if (!fp) {
        perror("failed to open input.txt");
        exit(EXIT_FAILURE);
    }

    list_ele_t *head = head_new();
    char buf[256];
    while (fgets(buf, 256, fp))
        insert_list(head, buf);
    fclose(fp);

    list_merge_sort(head);
    list_show(&head->list);

    list_free(&head->list);

    return 0;
}
