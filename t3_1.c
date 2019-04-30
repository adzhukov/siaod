// Hash table, collision resolved by separate chaining using array list.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char** key;
    char** value;
    size_t size;
    size_t used;
} List;


typedef struct table{
    List** list;
    size_t size;
    size_t used;
} hashTable;

hashTable* newHashTable(size_t);
void freeHashTable(hashTable*);
void addToHashTable(hashTable*, char*, char*);
void printHashTable(hashTable*);
void removeValueForKey(hashTable*, char*);
const char* getValueForKey(hashTable*, char*);
unsigned long getStringHash(char*);

List* newList();
void freeList(List*);
int addToList(List*, char*, char*);
void resizeList(List*);

hashTable* newHashTable(size_t size) {
    hashTable* ret = (hashTable*)malloc(sizeof(hashTable));
    ret->size = size;
    ret->list = (List**)malloc(ret->size * sizeof(List));
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
        List* list = table->list[i];
        for (size_t i = 0; i < list->used; i++)
            printf("  key: %s; value: %s\n", list->key[i], list->value[i]);
    }
}

const char* getValueForKey(hashTable* table, char* key) {
    unsigned long index = getStringHash(key) % table->size;
    List* list = table->list[index];
    for (size_t i = 0; i < list->used; i++)
        if (!strcmp(list->key[i], key))
            return list->value[i];
    return NULL;
}

void addToHashTable(hashTable* table, char* key, char* value) {
    unsigned long index = getStringHash(key) % table->size;
    table->used += addToList(table->list[index], key, value);
}

void removeValueForKey(hashTable* table, char* key) {
    unsigned long index = getStringHash(key) % table->size;
    List* list = table->list[index];
    for (size_t i = 0; i < list->used; i++) {
        if (!strcmp(list->key[i], key)) {
            free(list->key[i]);
            free(list->value[i]);
            list->used--;
            table->used--;
            return;
        }
    }
}

List* newList() {
    List* ret = (List*)malloc(sizeof(List));
    ret->size = 2;
    ret->key = (char**)malloc(sizeof(char*) * ret->size);
    ret->value = (char**)malloc(sizeof(char*) * ret->size);
    ret->used = 0;
    return ret;
}

void resizeList(List* list) {
    list->size *= 2;
    list->key = (char**)realloc(list->key, sizeof(char*) * list->size);
    list->value = (char**)realloc(list->value, sizeof(char*) * list->size);
}

int addToList(List* list, char* key, char* value) {
    size_t oldSize = list->used++;
    if (list->used == list->size)
        resizeList(list);
    for (size_t i = 0; i < oldSize; i++)
        if (!strcmp(key, list->key[i])) {
            list->value[i] = strdup(value);
            return 0;
        }
    list->key[oldSize] = strdup(key);
    list->value[oldSize] = strdup(value);
    return 1;
}

void freeList(List* list) {
    for (size_t i = 0; i < list->used; i++) {
        free(list->key[i]);
        free(list->value[i]);
    }
    free(list);
}

int main(int argc, char** argv) {
    hashTable* table = newHashTable(10);
    size_t maxStringLen = (argc == 2) ? atoi(argv[1]) : 256;
    char cmd[maxStringLen];
    puts("usage: a <key> <value> - add\n" \
         "       r <key> - remove\n" \
         "       f <key> - get value for key\n" \
         "       p - print table\n" \
         "       q - quit");
    while (1) {
        fgets(cmd, maxStringLen-1, stdin);
        char* token = strtok(cmd, " ");
        switch (cmd[0]) {
            case 'a': {
                char* key = strtok(NULL, " ");
                key = key ? key : "";
                char* value = strtok(NULL, "\n");
                value = value ? value : "";
                addToHashTable(table, key, value);
                printHashTable(table);
                break;
            }
            case 'r':
                removeValueForKey(table, strtok(NULL, "\n"));
                printHashTable(table);
                break;
            case 'f': {
                const char* value = getValueForKey(table, strtok(NULL, "\n"));
                value = value ? value : "Not Found";
                printf("%s\n", value);
                break;
            }
            case 'p':
                printHashTable(table);
                break;
            case 'q':
                freeHashTable(table);
                return 0;
            case 'c':
                freeHashTable(table);
                table = newHashTable(atoi(strtok(NULL, "\n")));
                break;
            default:
                break;
        }
    }
}
