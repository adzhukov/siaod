#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct linkedListNode {
    char* string;
    struct linkedListNode* next;
};

typedef struct {
  struct linkedListNode *first;
} LinkedList;

LinkedList* new_list();
void freeList(LinkedList *);
void add(LinkedList*, char *);
void printList(LinkedList*);
void removeFirst(LinkedList*, char*);
void find(LinkedList*, char*);

LinkedList* new_list() {
    LinkedList* ret = (LinkedList*)malloc(sizeof(LinkedList));
    ret->first = NULL;
    return ret;
}

void add(LinkedList* list, char* tempString) {
    char* value = (char*)malloc(sizeof(char) * strlen(tempString) + 1);
    strcpy(value, tempString);
    struct linkedListNode* prev = NULL;
    struct linkedListNode* newNode = (struct linkedListNode*)malloc(sizeof(struct linkedListNode));
    newNode->string = value;
    newNode->next = NULL;
    if (!list->first) {
        list->first = newNode;
        return;
    }
    struct linkedListNode* temp = list->first;
    while (1) {
        if (strcmp(temp->string, value)>0) {
            if (prev) {
                prev->next = newNode;
                newNode->next = temp;
            } else {
                newNode->next = list->first;
                list->first = newNode;
            }
            return;
        }
        if (temp->next) {
            prev = temp;
            temp = temp->next;
        } else {
            temp->next = newNode;
            return;
        }
    }
}

void freeNode(struct linkedListNode* node) {
    free(node->string);
    free(node);
}

void printList(LinkedList* _list) {
    struct linkedListNode* temp = _list->first;
    while (temp) {
        printf("%s; ", temp->string);
        temp = temp->next;
    }
    printf("\n");
}

void freeList(LinkedList* list) {
	struct linkedListNode* temp = list->first;
	struct linkedListNode* tempNext;
    while (temp) {
    	tempNext = temp->next;
        freeNode(temp);
        temp = tempNext;
    }
    free(list);
}

void removeFirst(LinkedList* list, char* value) {
    if (!list->first) return;
    struct linkedListNode* temp = list->first;
    struct linkedListNode* prev = NULL;
    while (temp) {
        if (!strcmp(temp->string, value)) {
            if (prev) {
                prev->next = temp->next;
                freeNode(temp);
            } else {
                list->first = (list->first)->next;
                freeNode(temp);
            }
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

void find(LinkedList* list, char* value) {
    struct linkedListNode* node = list->first;
    while (node) {
        int cmp = strcmp(value, node->string);
        if (!cmp) {
            printf("%s\n", value);
            return;
        } else if (cmp < 0) {
            break;
        }
        node = node->next;
    }
    printf("Not Found\n");
}

int main(int argc, char** argv) {
    LinkedList* list = new_list();
    size_t maxStringLen = (argc == 2) ? atoi(argv[1]) : 256;
    char cmd[maxStringLen];
    puts("usage: a <string> - add\n" \
         "       r <string> - removeFirst\n" \
         "       p - print list\n" \
         "       q - quit");
    while (1) {
        memset(cmd, 0, maxStringLen);
        fgets(cmd, maxStringLen-1, stdin);
        char* token = strtok(cmd, " ");
        switch (cmd[0]) {
            case 'a': {
                add(list, strtok(NULL, "\n"));
                printList(list);
                break;
            }
            case 'r': {
                removeFirst(list, strtok(NULL, "\n"));
                printList(list);
                break;
            }
            case 'f': {
                find(list, strtok(NULL, "\n"));
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
