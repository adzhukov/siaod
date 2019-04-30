// Hash table, collision resolved by separate chaining using binary search tree.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct treeNode {
    char* key;
    char* value;
    struct treeNode* left;
    struct treeNode* right;
} treeNode;

typedef struct {
  struct treeNode* root;
} Tree;

typedef struct {
    Tree** list;
    size_t size;
} hashTable;

hashTable* newHashTable(size_t);
void freeHashTable(hashTable*);
void addToHashTable(hashTable*, char*, char*);
void printHashTable(hashTable*);
void removeValueForKey(hashTable*, char*);
const char* getValueForKey(hashTable*, char*);
unsigned long getStringHash(char*);

Tree* newTree();
void freeTree(Tree*);
void addToTree(Tree*, char*, char*);
void freeTreeNode(treeNode*);
void removeFromTree(Tree*, char*);

void freeTreeNode(treeNode* node) {
    free(node->key);
    free(node->value);
    if (node->left)
        freeTreeNode(node->left);
    if (node->right)
        freeTreeNode(node->right);
    free(node);
}

void freeTree(Tree* tree) {
    if (tree->root)
        freeTreeNode(tree->root);
    free(tree);
}

Tree* newTree() {
    Tree* ret = (Tree*)malloc(sizeof(Tree));
    ret->root = NULL;
    return ret;
}

hashTable* newHashTable(size_t size) {
    hashTable* ret = (hashTable*)malloc(sizeof(hashTable));
    ret->size = size;
    ret->list = (Tree**)malloc(ret->size * sizeof(Tree));
    for (size_t i = 0; i < ret->size; i++)
        ret->list[i] = newTree();
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
        freeTree(table->list[i]);
    free(table);
}

void printTreeNode(struct treeNode* node) {
    printf("  key: %s; value: %s\n", node->key, node->value);
    if (node->left)
        printTreeNode(node->left);
    if (node->right)
        printTreeNode(node->right);
}

void printHashTable(hashTable* table) {
    for (size_t i = 0; i < table->size; i++) {
        treeNode* node = table->list[i]->root;
        if (node)
            printTreeNode(node);
    }
}

const char* getValueForKey(hashTable* table, char* key) {
    unsigned long index = getStringHash(key) % table->size;
    treeNode* node = table->list[index]->root;
    while (node) {
        int cmp = strcmp(node->key, key);
        if (!cmp) return node->value;
        node = (cmp > 0) ? node->left : node->right;
    }
    return NULL;
}

void addToHashTable(hashTable* table, char* key, char* value) {
    unsigned long index = getStringHash(key) % table->size;
    addToTree(table->list[index], key, value);
}

void removeValueForKey(hashTable* table, char* key) {
    unsigned long index = getStringHash(key) % table->size;
    removeFromTree(table->list[index], key);
}

static inline void fillTreeNode(struct treeNode* node, char* key, char* value) {
    node->left = node->right = NULL;
    node->key = strdup(key);
    node->value = strdup(value);
}

void addToTree(Tree* tree, char* key, char* value) {
    treeNode* node = tree->root;
    if (!node) {
        tree->root = (treeNode*)malloc(sizeof(treeNode));
        fillTreeNode(tree->root, key, value);
        return;
    }
    treeNode* prev = node;
    int cmp = 0;
    while (node) {
        cmp = strcmp(node->key, key);
        if (!cmp) {
            free(node->value);
            node->value = strdup(value);
            return;
        }
        prev = node;
        node = (cmp > 0) ? node->left : node->right;
    }
    if (cmp > 0) {
        prev->left = (treeNode*)malloc(sizeof(treeNode));
        fillTreeNode(prev->left, key, value);
    } else {
        prev->right = (treeNode*)malloc(sizeof(treeNode));
        fillTreeNode(prev->right, key, value);
    }
}

void removeFromTree(Tree* tree, char* key) {
    treeNode* node = tree->root;
    treeNode* prev = node;
    while (node) {
        int cmp = strcmp(node->key, key);
        if (!cmp) {
            if (!node->left && !node->right) {
                if (node == tree->root)
                    tree->root = NULL;
                if (node == prev->left)
                    prev->left = NULL;
                else
                    prev->right = NULL;
                freeTreeNode(node);
            } else if (node->left && node->right) {
                if (!node->right->left) {
                    node->key = node->right->key;
                    node->value = node->right->value;
                    treeNode* temp = node->right->right;
                    free(node->right);
                    node->right = temp;
                } else {
                    treeNode* temp = node->right;
                    while (temp->left->left)
                        temp = temp->left;
                    char* newKey = temp->left->key;
                    char* newValue = temp->left->value;
                    removeFromTree(tree, newKey);
                    node->key = newKey;
                    node->value = newValue;
                }
            } else {
                treeNode* temp = node->left;
                node->key = temp ? node->left->key : node->right->key;
                node->value = temp ? node->left->value : node->right->value;
                free(temp ? node->left : node->right);
                node->left = node->right = NULL;
            }
            return;
        }
        prev = node;
        node = (cmp > 0) ? node->left : node->right;
    }
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
            case 'r': {
                char* key = strtok(NULL, "\n");
                key = key ? key : "";
                removeValueForKey(table, key);
                printHashTable(table);
                break;
            }
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
