#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char** first;
    size_t size;
    size_t used;
} List;

List* newList();
void printList(List*);
void find(List*, char*);
void freeList(List*);
void resizeList(List*);
void addToList(List*, char*);
void addToListBefore(List*, char*, char*);
void addToListAfter(List*, char*, char*);
void removeFirst(List*, char*);
void removeAll(List*, char*);

List* newList() {
    List* ret = (List*)malloc(sizeof(List));
    ret->size = 2;
    ret->first = (char**)malloc(sizeof(char*) * ret->size);
    ret->used = 0;
    return ret;
}

void printList(List* list) {
    printf("List size: %lu; used: %lu\n", list->size, list->used);
    for (size_t i = 0; i < list->used; i++)
        printf("[%lu] %s\n", i, list->first[i]);
}

void find(List* list, char* value) {
    for (size_t i = 0; i < list->used; i++)
        if (!strcmp(list->first[i], value)) {
            printf("[%lu] %s\n", i, list->first[i]);
            return;
        }
    printf("[-] Not found\n");
}

void freeList(List* list) {
    for (size_t i = 0; i < list->used; i++)
        free(list->first[i]);
    free(list);
}

void resizeList(List* list) {
    list->size *= 2;
    list->first = (char**)realloc(list->first, sizeof(char*) * list->size);
}

void addToList(List* list, char* value) {
    size_t i = 0;
    size_t oldSize = list->used++;
    if (list->used == list->size)
        resizeList(list);
    for (; i < oldSize; i++)
        if (strcmp(value, list->first[i]) < 0)
            break;
    for (size_t j = oldSize ; j > i; j--)
        list->first[j] = list->first[j - 1];
    list->first[i] = (char*)malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(list->first[i], value);
    return;
}

void addToListBefore(List* list, char* value, char* before) {
    size_t oldSize = list->used++;
    if (list->used == list->size)
        resizeList(list);
    for (size_t i = 0; i < oldSize; i++)
        if (!strcmp(before, list->first[i])) {
            for (size_t j = oldSize; j > i; j--)
                list->first[j] = list->first[j-1];
            list->first[i] = (char*)malloc(sizeof(char) * (strlen(value) + 1));
            strcpy(list->first[i], value);
            return;
        }
    list->first[oldSize] = (char*)malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(list->first[oldSize], value);
}

void addToListAfter(List* list, char* value, char* after) {
    size_t oldSize = list->used++;
    if (list->used == list->size)
        resizeList(list);
    for (size_t i = 0; i < oldSize; i++)
        if (!strcmp(after, list->first[i])) {
            i++;
            for (size_t j = oldSize; j > i; j--)
                list->first[j] = list->first[j - 1];
            list->first[i] = (char*)malloc(sizeof(char) * (strlen(value) + 1));
            strcpy(list->first[i], value);
            return;
        }
    list->first[oldSize] = (char*)malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(list->first[oldSize], value);
}


void removeFirst(List* list, char* value) {
    for (size_t i = 0; i < list->used; i++)
        if (!strcmp(list->first[i], value)) {
            free(list->first[i]);
            for (size_t j = i; j < list->used - 1; j++)
                list->first[j] = list->first[j + 1];
            list->used--;
            return;
        }
}

void removeAll(List* list, char* value) {
    for (size_t i = 0; i < list->used; i++)
        if (!strcmp(list->first[i], value)) {
            free(list->first[i]);
            for (size_t j = i; j < list->used - 1; j++)
                list->first[j] = list->first[j + 1];
            i--;
            list->used--;
        }
}

int main(int argc, char** argv) {
    List* list = newList();
    size_t maxStringLen = (argc == 2) ? atoi(argv[1]) : 256;
    char cmd[maxStringLen];
    puts("usage: a <string> - add\n" \
         "       s <string> <before> - add before\n" \
         "       d <string> <after> - add after\n" \
         "       r <string> - remove first\n" \
         "       t <string> - remove all\n" \
         "       f <string> - find\n" \
         "       p - print list\n" \
         "       q - quit");
    while (1) {
        memset(cmd, 0, maxStringLen);
        fgets(cmd, maxStringLen-1, stdin);
        char* token = strtok(cmd, " ");
        switch (cmd[0]) {
            case 'a':
                addToList(list, strtok(NULL, " \n"));
                break;
            case 's': {
                char* value = strtok(NULL, " \n");
                value = value ? value : "";
                char* before = strtok(NULL, " \n");
                before = before ? before : "";
                addToListBefore(list, value, before);
                break;
            }
            case 'd': {
                char* value = strtok(NULL, " \n");
                value = value ? value : "";
                char* after = strtok(NULL, " \n");
                after = after ? after : "";
                addToListAfter(list, value, after);
                break;
            }
            case 'r': {
                char* value = strtok(NULL, " \n");
                value = value ? value : "";
                removeFirst(list, value);
                break;
            }
            case 't': {
                char* value = strtok(NULL, " \n");
                value = value ? value : "";
                removeAll(list, value);
                break;
            }
            case 'f': {
                char* value = strtok(NULL, " \n");
                value = value ? value : "";
                find(list, value);
                break;
            }
            case 'p':
                printList(list);
                break;
            case 'q':
                freeList(list);
                return 0;
            default:
                break;
        }
    }
}
