#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct linkedListNode {
    char* key;
    char* value;
    struct linkedListNode* next;
};

typedef struct {
  struct linkedListNode *first;
} LinkedList;

LinkedList* newList();
void freeList(LinkedList *);
void addToList(LinkedList*, char*, char*);
void freeNode(struct linkedListNode* );

typedef struct table{
    LinkedList** list;
    size_t size;
    size_t used;
} hashTable;

hashTable* newHashTable();
void freeHashTable(hashTable*);
void addToHashTable(hashTable*, char*, char*);
void printHashTable(hashTable*);
void removeFromTable(hashTable*, char*, char*);
unsigned long getStringHash(char*);

hashTable* newHashTable() {
    hashTable* ret = (hashTable*)malloc(sizeof(hashTable));
    ret->size = 2;
    ret->list = (LinkedList**)calloc(ret->size, sizeof(LinkedList));
    ret->used = 0;
    return ret;
}

unsigned long getStringHash(char* value) {
    unsigned long hash = 7;
    for (size_t i = 0; i < strlen(value); i++)
        hash = hash * 31 + value[i];
    return hash;
}

void freeHashTable(hashTable* table) {
    for (size_t i = 0; i < table->size; i++)
        freeList(table->list[i]);
    free(table);
}

void printHashTable(hashTable* table) {
    for (size_t i = 0; i < table->size; i++) {
        printf("hash: %zu\n", i);
        if (table->list[i]) {
            struct linkedListNode* node = table->list[i]->first;
            while (node) {
                printf("  key: %s; value: %s", node->key, node->value);
                node = node->next;
            }
        }
    }
}

void resizeHashTable(hashTable* table) {
    table->size <<= 1;
    table->list = (LinkedList**)realloc(table->list, sizeof(LinkedList) * table->size);
    memset(&(table->list[table->size >> 1]), 0, sizeof(LinkedList) * (table->size >> 1));
    for (size_t i = 0; i < table->size >> 1; i++) {
        LinkedList* list = table->list[i];
        if (!list) continue;
        struct linkedListNode* node = list->first;
        struct linkedListNode* prevNode = NULL;
        while (node) {
            if (getStringHash(node->key) % (table->size) >= table->size >> 1) {
                addToHashTable(table, node->key, node->value);
                if (prevNode) {
                    prevNode->next = node->next;
                } else {
                    list->first = list->first->next;
                }
                freeNode(node);
            }
            prevNode = node;
            node = node->next;
        }
    }
}

void addToHashTable(hashTable* table, char* key, char* value) {
    unsigned long hash = getStringHash(key);
    unsigned long index = getStringHash(key) % table->size;
    if (!table->list[index]) {
        table->list[index] = newList();
    }
    addToList(table->list[index], key, value);
    table->used++;
    if ((float)(table->used)/(float)(table->size) > 0.5) {
        resizeHashTable(table);
    }
}

void removeFromTable(hashTable* table, char* key, char* value) {
    unsigned long hash = getStringHash(key);
    unsigned long index = getStringHash(key) % table->size;
    LinkedList* list = table->list[index];
    if (!list) return;
    struct linkedListNode* node = list->first;
    struct linkedListNode* prevNode = NULL;
    while (node) {
        if (!strcmp(key, node->key) && !strcmp(value, node->value)) {
            if (prevNode) {
                prevNode->next = node->next;
            } else {
                list->first = list->first->next;
            }
            freeNode(node);
            table->used--;
            return;
        }
        prevNode = node;
        node = node->next;
    }
}

LinkedList* newList() {
    return (LinkedList*)calloc(1, sizeof(LinkedList));
}

void addToList(LinkedList* list, char* tempKey, char* tempValue) {
    char* value = (char*)malloc(sizeof(char) * (strlen(tempValue) + 1));
    char* key = (char*)malloc(sizeof(char) * (strlen(tempKey) + 1));
    strcpy(value, tempValue);
    strcpy(key, tempKey);
    struct linkedListNode* prev = NULL;
    struct linkedListNode* newNode = (struct linkedListNode*)malloc(sizeof(struct linkedListNode));
    newNode->value = value;
    newNode->key = key;
    newNode->next = NULL;
    if (!list->first) {
        list->first = newNode;
        return;
    }
    struct linkedListNode* temp = list->first;
    while (1) {
        if (strcmp(temp->value, value) > 0) {
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
    free(node->key);
    free(node->value);
    free(node);
}

void freeList(LinkedList* list) {
    if (!list) return;
    struct linkedListNode* temp = list->first;
    struct linkedListNode* tempNext;
    while (temp) {
    	tempNext = temp->next;
        freeNode(temp);
        temp = tempNext;
    }
    free(list);
}

int main(int argc, char** argv) {
    hashTable* table = newHashTable();
    size_t maxStringLen = (argc == 2) ? atoi(argv[1]) : 256;
    char cmd[maxStringLen];
    puts("usage: a <key> <value> - add\n" \
         "       r <key> <value> - remove\n" \
         "       p - print table\n" \
         "       q - quit");
    while (1) {
        memset(cmd, 0, maxStringLen);
        fgets(cmd, maxStringLen-1, stdin);
        char* token = strtok(cmd, " ");
        switch (cmd[0]) {
            case 'a': {
                char* key = strtok(NULL, " ");
                char* value = strtok(NULL, " ");
                addToHashTable(table, key, value);
                break;
            }
            case 'r': {
                char* key = strtok(NULL, " ");
                char* value = strtok(NULL, " ");
                removeFromTable(table, key, value);
                break;
            }
            case 'p':
                printHashTable(table);
                break;
            case 'q':
            	freeHashTable(table);
                return 0;
            default:
                break;
        }
    }
}

#ifdef __cplusplus
}
#endif
