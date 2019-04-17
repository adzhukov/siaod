// Hash table, collision resolved by open addressing with linear probing.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct table{
    char** key;
    char** value;
    int* tombstone;
    size_t size;
} hashTable;

hashTable* newHashTable(size_t);
void freeHashTable(hashTable*);
int addValueForKey(hashTable*, const char*, const char*);
void printHashTable(hashTable*);
void removeValueForKey(hashTable*, char*);
void removeValueForKeyReorder(hashTable*, char*);
const char* getValueForKey(hashTable*, const char*);
unsigned long getStringHash(const char*);

hashTable* newHashTable(size_t size) {
    hashTable* ret = (hashTable*)malloc(sizeof(hashTable));
    ret->size = size;
    ret->key = (char**)malloc(sizeof(char*) * size);
    ret->value = (char**)malloc(sizeof(char*) * size);
    ret->tombstone = (int*)malloc(sizeof(int) * size);
    for (size_t i = 0; i < size; i++)
        ret->tombstone[i] = 0;
    return ret;
}

unsigned long getStringHash(const char* value) {
    unsigned long hash = 7;
    for (size_t i = 0; i < strlen(value); i++)
        hash = hash * 31 + value[i];
    return hash;
}

void freeHashTable(hashTable* table) {
    for (size_t i = 0; i < table->size; i++) {
        if (table->tombstone[i]) {
            free(table->key[i]);
            free(table->value[i]);
        }
    }
    free(table->tombstone);
    free(table);
}

void printHashTable(hashTable* table) {
    for (size_t i = 0; i < table->size; i++) {
        if (table->tombstone[i]) {
            printf("  key: %s; value: %s\n", table->key[i], table->value[i]);
        }
    }
}

const char* getValueForKey(hashTable* table, const char* key) {
    size_t index = getStringHash(key) % table->size;
    for (size_t i = index; i < table->size; i++)
        if (table->tombstone[i] && !strcmp(table->key[i], key))
            return table->value[i];
    return NULL;
}

//  return 0 on success, any other value otherwise
int addValueForKey(hashTable* table, const char* key, const char* value) {
    unsigned long index = getStringHash(key) % table->size;
    for (size_t i = index; i < table->size; i++) {
        if (table->tombstone[i]) {
            if (!strcmp(table->key[i], key)) {
                free(table->value[i]);
                table->value[i] = (char*)malloc(sizeof(char) * (strlen(key) + 1));
                strcpy(table->value[i], value);
                return 0;
            }
        } else {
            table->key[i] = (char*)malloc(sizeof(char) * (strlen(key) + 1));
            strcpy(table->key[i], key);
            table->value[i] = (char*)malloc(sizeof(char) * (strlen(key) + 1));
            strcpy(table->value[i], value);
            table->tombstone[i] = 1;
            return 0;
        }
    }
    return 1;
}

void removeValueForKey(hashTable* table, char* key) {
    unsigned long index = getStringHash(key) % table->size;
    for (size_t i = 0; i < table->size; i++)
        if (table->tombstone[i] && !strcmp(table->key[i], key)) {
            free(table->key[i]);
            free(table->value[i]);
            table->tombstone[i] = 0;
            return;
        }
}

void removeValueForKeyReorder(hashTable* table, char* key) {
    unsigned long index = getStringHash(key) % table->size;
    for (size_t i = 0; i < table->size; i++)
        if (table->tombstone[i] && !strcmp(table->key[i], key)) {
            free(table->key[i]);
            free(table->value[i]);
            table->tombstone[i] = 0;
            for (size_t j = i + 1; j < table->size; j++)
                if (table->tombstone[j]) {
                    unsigned long position = getStringHash(table->key[j]) % table->size;
                    for (size_t newPosition = position; newPosition < j; newPosition++)
                        if (!table->tombstone[newPosition]) {
                            table->tombstone[newPosition] = 1;
                            table->key[newPosition] = table->key[j];
                            table->value[newPosition] = table->value[j];
                            table->tombstone[j] = 0;
                            break;
                        }
                }
            return;
        }
}

int main(int argc, char** argv) {
    hashTable* table = newHashTable(10);
    size_t maxStringLen = 256;
    char input[maxStringLen];
    puts("usage: a <key> <value> - add value for key\n" \
         "       r <key> - remove value for key\n" \
         "       t <key> - remove value for key & reorder\n" \
         "       f <key> - get value for key\n" \
         "       p - print table\n" \
         "       q - quit");
    while (1) {
        fgets(input, maxStringLen-1, stdin);
        char* token = strtok(input, " ");
        switch (input[0]) {
            case 'a': {
                const char* key = strtok(NULL, " ");
                key = key ? key : "";
                const char* value = strtok(NULL, "\n");
                value = value ? value : "";
                if (addValueForKey(table, key, value))
                    puts("ERROR: Unable to add new key value pair, table overflow");
                else
                    printHashTable(table);
                break;
            }
            case 'r':
                removeValueForKey(table, strtok(NULL, "\n"));
                printHashTable(table);
                break;
            case 't':
                removeValueForKeyReorder(table, strtok(NULL, "\n"));
                printHashTable(table);
                break;
            case 'f': {
                const char* key = strtok(NULL, "\n");
                key = key ? key : "";
                const char* value = getValueForKey(table, key);
                if (value)
                    printf("value for key %s: %s\n", key, value);
                else
                    printf("ERROR: value for key %s not found\n", key);
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
