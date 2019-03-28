#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMP <

struct node {
    struct node* prev;
    struct node* left;
    struct node* right;
    char* string;
    int color;
};

enum { black = 0, red = 1};

typedef struct {
    struct node* root;
} RBTree;

RBTree* newRBTree();
void find(RBTree*, char*);
void rotateLeft(RBTree*, struct node*);
void rotateRight(RBTree*, struct node*);
void add(RBTree*, char*);
void freeTree(RBTree*);
void printTree(RBTree*);

RBTree* newRBTree() {
    return (RBTree*)calloc(1, sizeof(RBTree));
}

void find(RBTree* tree, char* value) {
    struct node* temp = tree->root;
    while (temp) {
        if (!strcmp(value, temp->string)) {
            printf("%c %s\n", (temp->color == red) ? 'R' : 'B', temp->string);
            return;
        }
        temp = (strcmp(value, temp->string) CMP 0) ? temp->left : temp->right;
    }
    printf("Not Found\n");
}

void rotateLeft(RBTree* tree, struct node* node) {
    struct node* rightNode = node->right;
    node->right = rightNode->left;
    if (rightNode->left)
        rightNode->left->prev = node;
    if (rightNode)
        rightNode->prev = node->prev;
    if (node->prev) {
        if (node == node->prev->left)
            node->prev->left = rightNode;
        else
            node->prev->right = rightNode;
    } else {
        tree->root = rightNode;
    }
    rightNode->left = node;
    if (node)
        node->prev = rightNode;
}

void rotateRight(RBTree* tree, struct node* node) {
    struct node* leftNode = node->left;
    node->left = leftNode->right;
    if (leftNode->right)
        leftNode->right->prev = node;
    if (leftNode)
        leftNode->prev = node->prev;
    if (node->prev) {
        if (node == node->prev->right)
            node->prev->right = leftNode;
        else
            node->prev->left = leftNode;
    } else {
        tree->root = leftNode;
    }
    leftNode->right = node;
    if (node)
        node->prev = leftNode;
}

void add(RBTree* tree, char* tempString) {
    char* value = (char*)malloc(sizeof(char) * strlen(tempString) + 1);
    strcpy(value, tempString);
    struct node* newNode= (struct node*)calloc(1, sizeof(struct node));
    newNode->string = value;
    struct node* temp = tree->root;
    while (temp) {
#ifndef ADD
        if (!strcmp(value, temp->string)) break;
#else
#undef ADD
#endif
        newNode->prev = temp;
        temp = (strcmp(value, temp->string) CMP 0) ? temp->left : temp->right;
    }
    if (newNode->prev) {
        if (strcmp(value, (newNode->prev)->string) CMP 0)
            newNode->prev->left = newNode;
        else
            newNode->prev->right = newNode;
        
        newNode->color = red;
        while (newNode->prev && newNode->prev->color == red && newNode->color == red && newNode->prev->prev) {
            struct node* prevNode = newNode->prev;
            struct node* prevPrevNode = prevNode->prev;
            int isPrevNodeLeft = prevNode == prevPrevNode->left;
            struct node* adjacentNode = isPrevNodeLeft ? prevPrevNode->right : prevPrevNode->left;
            if (adjacentNode && adjacentNode->color == red) {
                if (prevPrevNode != tree->root) prevPrevNode->color = red;
                prevNode->color = black;
                adjacentNode->color = black;
                newNode = prevPrevNode;
            } else {
                if (newNode == (isPrevNodeLeft ? prevNode->right : prevNode->left)) {
                    (isPrevNodeLeft ? rotateLeft : rotateRight)(tree, prevNode);
                    newNode = prevNode;
                    prevNode = newNode->prev;
                } else {
                    (isPrevNodeLeft ? rotateRight : rotateLeft)(tree, prevPrevNode);
                    int tempColor=prevNode->color;
                    prevNode->color=prevPrevNode->color;
                    prevPrevNode->color=tempColor;
                    newNode = prevNode;
                }
            }
        }
    } else {
        tree->root = newNode;
    }
}

void _freeTree(struct node* root) {
    if (root->left) {
        _freeTree(root->left);
    }
    if (root->right) {
        _freeTree(root->right);
    }
    free(root->string);
    free(root);
}

void freeTree(RBTree* tree) {
    if (tree->root) _freeTree(tree->root);
    free(tree);
}

void structure(struct node* root, int level) {
    if (!root) {
        for (size_t i = 0; i < level; i++)
            putchar('\t');
        puts("~");
    } else {
        structure(root->right, level + 1);
        for (size_t i = 0; i < level; i++)
            putchar('\t');
        printf("%c %s\n", (root->color == red) ? 'R' : 'B', root->string);
        structure(root->left, level + 1);
    }
}

void printTree(RBTree* tree) {
    structure(tree->root, 0);
}

#undef CMP

int main(int argc, char** argv) {
    RBTree* tree = newRBTree();
    size_t maxStringLen = (argc == 2) ? atoi(argv[1]) : 256;
    char cmd[maxStringLen];
    puts("usage: a <string> - add\n" \
         "       f <string> - find\n" \
         "       p - print tree\n" \
         "       q - quit");
    while (1) {
        memset(cmd, 0, maxStringLen);
        fgets(cmd, maxStringLen-1, stdin);
        strtok(cmd, "\n");
        switch (cmd[0]) {
            case 'a': {
                add(tree, cmd+2);
                printTree(tree);
                break;
            }
            case 'p':
                printTree(tree);
                break;
            case 'f':
                find(tree, cmd+2);
                break;
            case 'q':
                freeTree(tree);
                return 0;
            default:
                break;
        }
    }
}

#ifdef __cplusplus
}
#endif
