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

typedef struct table{
    LinkedList** list;
    size_t size;
    size_t used;
} hashTable;

hashTable* newHashTable();
void freeHashTable(hashTable*);
void addToHashTable(hashTable*, char*, char*);
void printHashTable(hashTable*);
void removeValueForKey(hashTable*, char*);
const char* getValueForKey(hashTable*, char*);
unsigned long getStringHash(char*);

LinkedList* newList();
void freeList(LinkedList *);
int addToList(LinkedList*, char*, char*);
void freeNode(struct linkedListNode* );

hashTable* newHashTable() {
    hashTable* ret = (hashTable*)malloc(sizeof(hashTable));
    ret->size = 2;
    ret->list = (LinkedList**)malloc(ret->size * sizeof(LinkedList));
    for (size_t i = 0; i < ret->size; i++)
        ret->list[i] = newList();
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
    printf("table size: %lu; used: %lu;\n", table->size, table->used);
    for (size_t i = 0; i < table->size; i++) {
        printf("hash: %lu\n", i);
        struct linkedListNode* node = table->list[i]->first;
        while (node) {
            printf("  key: %s; value: %s\n", node->key, node->value);
            node = node->next;
        }
    }
}

const char* getValueForKey(hashTable* table, char* key) {
    for (size_t i = 0; i < table->size; i++) {
        struct linkedListNode* node = table->list[i]->first;
        while (node) {
            if (!strcmp(key, node->key)) {
                return node->value;
            }
            node = node->next;
        }
    }
    return "";
}

void resizeHashTable(hashTable* table) {
    size_t oldSize = table->size;
    table->size <<= 1;
    table->list = (LinkedList**)realloc(table->list, sizeof(LinkedList) * table->size);
    for (size_t i = oldSize; i < table->size; i++)
        table->list[i] = newList();
    for (size_t i = 0; i < oldSize; i++) {
        LinkedList* list = table->list[i];
        struct linkedListNode* node = list->first;
        struct linkedListNode* prevNode = NULL;
        while (node) {
            unsigned long newIndex = getStringHash(node->key) % table->size;
            if (newIndex >= oldSize) {
                table->list[newIndex] = newList();
                addToList(table->list[newIndex], node->key, node->value);
                if (prevNode) {
                    prevNode->next = node->next;
                } else {
                    list->first = list->first->next;
                }
                struct linkedListNode* temp = node->next;
                freeNode(node);
                node = temp;
            } else {
                prevNode = node;
                node = node->next;
            }
        }
    }
}

void addToHashTable(hashTable* table, char* key, char* value) {
    unsigned long index = getStringHash(key) % table->size;
    if (addToList(table->list[index], key, value))
        table->used++;
    if ((float)(table->used)/(float)(table->size) > 0.5)
        resizeHashTable(table);
}

void removeValueForKey(hashTable* table, char* key) {
    unsigned long index = getStringHash(key) % table->size;
    LinkedList* list = table->list[index];
    struct linkedListNode* node = list->first;
    struct linkedListNode* prevNode = NULL;
    while (node) {
        if (!strcmp(key, node->key)) {
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
    LinkedList* ret = (LinkedList*)malloc(sizeof(LinkedList));
    ret->first = NULL;
    return ret;
}

int addToList(LinkedList* list, char* tempKey, char* tempValue) {
    char* value = (char*)malloc(sizeof(char) * (strlen(tempValue) + 1));
    strcpy(value, tempValue);
    struct linkedListNode* temp = list->first;
    while (temp) {
        if (!strcmp(tempKey, temp->key)) {
            free(temp->value);
            temp->value = value;
            return 0;
        }
        temp = temp->next;
    }
    char* key = (char*)malloc(sizeof(char) * (strlen(tempKey) + 1));
    strcpy(key, tempKey);
    struct linkedListNode* newNode = (struct linkedListNode*)malloc(sizeof(struct linkedListNode));
    newNode->value = value;
    newNode->key = key;
    newNode->next = list->first;
    list->first = newNode;
    return 1;
}

void freeNode(struct linkedListNode* node) {
    free(node->key);
    free(node->value);
    free(node);
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

int main(int argc, char** argv) {
    hashTable* table = newHashTable();
    size_t maxStringLen = (argc == 2) ? atoi(argv[1]) : 256;
    char cmd[maxStringLen];
    puts("usage: a <key> <value> - add\n" \
         "       r <key> - remove\n" \
         "       f <key> - get value for key\n" \
         "       p - print table\n" \
         "       q - quit");
    while (1) {
        memset(cmd, 0, maxStringLen);
        fgets(cmd, maxStringLen-1, stdin);
        char* token = strtok(cmd, " ");
        switch (cmd[0]) {
            case 'a': {
                char* key = strtok(NULL, " ");
                char* value = strtok(NULL, "\n");
                addToHashTable(table, key, value);
                printHashTable(table);
                break;
            }
            case 'r':
                removeValueForKey(table, strtok(NULL, "\n"));
                printHashTable(table);
                break;
            case 'f':
                printf("%s\n", getValueForKey(table, strtok(NULL, "\n")));
                break;
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
