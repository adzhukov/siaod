#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct table{
    char** key;
    char** value;
    size_t size;
} hashTable;

hashTable* newHashTable(size_t);
void freeHashTable(hashTable*);
int addValueForKey(hashTable*, const char*, const char*);
void printHashTable(hashTable*);
void removeValueForKey(hashTable*, char*);
const char* getValueForKey(hashTable*, const char*);
size_t getStringHash(const char*);

hashTable* newHashTable(size_t size) {
    hashTable* ret = (hashTable*)malloc(sizeof(hashTable));
    ret->size = size;
    ret->key = (char**)malloc(sizeof(char*) * size);
    for (size_t i = 0; i < size; i++)
        ret->key[i] = NULL;
    ret->value = (char**)malloc(sizeof(char*) * size);
    return ret;
}

size_t getStringHash(const char* value) {
    size_t hash = 7;
    for (size_t i = 0; i < strlen(value); i++)
        hash = hash * 31 + value[i];
    return hash;
}

void freeHashTable(hashTable* table) {
    for (size_t i = 0; i < table->size; i++) {
        if (table->key[i]) {
            free(table->key[i]);
            free(table->value[i]);
        }
    }
    free(table);
}

void printHashTable(hashTable* table) {
    for (size_t i = 0; i < table->size; i++) {
        if (table->key[i]) {
            printf("  %lu. key: %s; value: %s\n", i, table->key[i], table->value[i]);
        }
    }
}

const char* getValueForKey(hashTable* table, const char* key) {
    const size_t index = getStringHash(key) % table->size;
    size_t current = index;
    do {
        if (!table->key[current])
            return NULL;
        if (!strcmp(table->key[current], key))
            return table->value[current];
        current = (current + 1) % table->size;
    } while (current != index);
    return NULL;
}

int addValueForKey(hashTable* table, const char* key, const char* value) {
    const size_t index = getStringHash(key) % table->size;
    size_t current = index;
    do {
        if (table->key[current]) {
            if (!strcmp(table->key[current], key)) {
                free(table->value[current]);
                table->value[current] = strdup(value);
                return 0;
            }
        } else {
            table->key[current] = strdup(key);
            table->value[current] = strdup(value);
            return 0;
        }
        current = (current + 1) % table->size;
    } while (current != index);
    return 1;
}

static inline size_t offset(size_t from, size_t to, size_t table_size) {
    return (from > to) ? table_size - to + from : to - from;
}

void removeValueForKey(hashTable* table, char* key) {
    const size_t index = getStringHash(key) % table->size;
    size_t current = index;
    do {
        if (!table->key[current])
            return;
        if (table->key[current] && !strcmp(table->key[current], key)) {
            free(table->key[current]);
            table->key[current] = NULL;
            free(table->value[current]);
            size_t pos = (current + 1) % table->size;
            size_t first_null = current;
            do {
                if (!table->key[pos])
                    break;
                const size_t hash = getStringHash(table->key[pos]) % table->size;
                if ((hash <= first_null && pos >= index) || (offset(first_null, pos, table->size) <= offset(hash, pos, table->size) && pos < index)) {
                    table->key[first_null] = table->key[pos];
                    table->value[first_null] = table->value[pos];
                    table->key[pos] = NULL;
                    first_null = pos;
                }
                pos = (pos + 1) % table->size;
            } while (pos != current);
            return;
        }
        current = (current + 1) % table->size;
    } while (current != index);
}

int main(int argc, char** argv) {
    hashTable* table = newHashTable(10);
    size_t maxStringLen = 256;
    char input[maxStringLen];
    while (1) {
        puts("\nusage: a <key> <value> - add value for key\n" \
             "       r <key> - remove value for key\n" \
             "       f <key> - get value for key\n" \
             "       p - print table\n" \
             "       q - quit");
        fgets(input, maxStringLen-1, stdin);
        char* token = strtok(input, " ");
        switch (input[0]) {
            case 'a': {
                const char* key = strtok(NULL, " \n");
                key = key ? key : "";
                const char* value = strtok(NULL, "\n");
                value = value ? value : "";
                if (addValueForKey(table, key, value))
                    puts("ERROR: Unable to add new key value pair, table overflow");
                else
                    printHashTable(table);
                break;
            }
            case 'r': {
                char *key = strtok(NULL, "\n");
                key = key ? key : "";
                removeValueForKey(table, key);
                printHashTable(table);
                break;
            }
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
            default:
                break;
        }
    }
}
