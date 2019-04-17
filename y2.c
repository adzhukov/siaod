/*      Print tree vertically
 *
 *      Example:
 *      A
 *      +-B
 *      | +-C
 *      | \-D
 *      \-E
 *        \-F
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    struct node** nodes;
    char* string;
};

typedef struct {
    struct node* root;
} Tree;

struct printTreeStruct {
    char** padding;
    char** value;
    size_t nodesCount;
    size_t treeHeight;
};

Tree* newTree();
void freeTree(Tree*);
void printTreeStruct(struct printTreeStruct*);
size_t subtreeNodesCount(struct node*);
size_t getNodesCount(Tree*);
size_t subtreeHeight(struct node*);
size_t getTreeHeight(Tree*);
void printTreeStruct(struct printTreeStruct*);
void freeTreePrintStruct(struct printTreeStruct*);
void fill(Tree* tree, struct printTreeStruct*);

Tree* newTree() {
    Tree* ret = (Tree*)malloc(sizeof(Tree));
    ret->root = NULL;
    return ret;
}

void makeTestTree(Tree* tree) {
    tree->root = (struct node*)malloc(sizeof(struct node));
    tree->root->string = "A";
    struct node** nodes = tree->root->nodes = (struct node**)malloc(sizeof(struct node*) * 3);
    
    nodes[0] = (struct node*)malloc(sizeof(struct node*));
    nodes[0]->nodes = (struct node**)malloc(sizeof(struct node*) * 3);
    nodes[0]->nodes[2] = NULL;
    nodes[0]->string = "B";
    
    nodes[0]->nodes[0] = (struct node*)malloc(sizeof(struct node*));
    nodes[0]->nodes[0]->nodes = (struct node**)malloc(sizeof(struct node*));
    nodes[0]->nodes[0]->nodes[0] = NULL;
    nodes[0]->nodes[0]->string = "C";
    
    nodes[0]->nodes[1] = (struct node*)malloc(sizeof(struct node*));
    nodes[0]->nodes[1]->nodes = (struct node**)malloc(sizeof(struct node*));
    nodes[0]->nodes[1]->nodes[0] = NULL;
    nodes[0]->nodes[1]->string = "D";
    
    nodes[1] = (struct node*)malloc(sizeof(struct node*));
    nodes[1]->nodes = (struct node**)malloc(sizeof(struct node*) * 2);
    nodes[1]->nodes[1] = NULL;
    nodes[1]->string = "E";
    
    nodes[1]->nodes[0] = (struct node*)malloc(sizeof(struct node*));
    nodes[1]->nodes[0]->nodes = (struct node**)malloc(sizeof(struct node*));
    nodes[1]->nodes[0]->nodes[0] = NULL;
    nodes[1]->nodes[0]->string = "F";
    
    nodes[2] = NULL;
}

void _freeTree(struct node* node) {
    size_t counter = 0;
    while (node->nodes[counter]) {
        _freeTree(node->nodes[counter]);
        counter++;
    }
    free(node->nodes);
    free(node);
}

void freeTree(Tree* tree) {
    _freeTree(tree->root);
    free(tree);
}

size_t subtreeNodesCount(struct node* node) {
    size_t ret = 1;
    size_t counter = 0;
    while (node->nodes[counter]) {
        ret += subtreeNodesCount(node->nodes[counter]);
        counter += 1;
    }
    return ret;
}

size_t getNodesCount(Tree* tree) {
    return subtreeNodesCount(tree->root);
}

size_t subtreeHeight(struct node* node) {
    size_t ret = 1;
    size_t counter = 0;
    size_t maxSubtreeHeight = 0;
    while (node->nodes[counter]) {
        size_t temp = subtreeHeight(node->nodes[counter]);
        maxSubtreeHeight = (temp > maxSubtreeHeight) ? temp : maxSubtreeHeight;
        counter++;
    }
    ret += maxSubtreeHeight;
    return ret;
}

size_t getTreeHeight(Tree* tree) {
    return subtreeHeight(tree->root);
}

void printTreeStruct(struct printTreeStruct* printTreeStruct) {
    printf("%s\n", printTreeStruct->value[0]);
    for (size_t i = 1; i < printTreeStruct->nodesCount; i++) {
        printf("%s", printTreeStruct->padding[i - 1]);
        printf("%s\n", printTreeStruct->value[i]);
    }
}

struct printTreeStruct* initPrint(Tree* tree) {
    struct printTreeStruct* ret = (struct printTreeStruct*)malloc(sizeof(struct printTreeStruct));
    ret->nodesCount = getNodesCount(tree);
    ret->treeHeight = getTreeHeight(tree);
    ret->value = (char**)malloc(sizeof(char*) * ret->nodesCount);
    ret->padding = (char**)malloc(sizeof(char*) * ret->nodesCount);
    for (size_t i = 0; i < ret->nodesCount; i++) {
        ret->padding[i] = (char*)malloc(sizeof(char) * 2 * ret->treeHeight + 1);
        memset(ret->padding[i], ' ', ret->treeHeight * 2);
    }
    return ret;
}

void freeTreePrintStruct(struct printTreeStruct* ret) {
    for (size_t i = 0; i < ret->nodesCount; i++)
        free(ret->padding[i]);
    free(ret->padding);
    free(ret->value);
    free(ret);
}

void _fill(struct node* node, struct printTreeStruct* printTreeStruct, size_t height, size_t row) {
    size_t counter = 0;
    printTreeStruct->value[row] = node->string;
    while (node->nodes[counter]) {
        size_t nodesCount = subtreeNodesCount(node->nodes[counter]);
        if (node->nodes[counter + 1]) {
            sprintf(printTreeStruct->padding[row] + height * 2, "+-");
            for (size_t i = row + 1; i < row + nodesCount; i++)
                sprintf(printTreeStruct->padding[i] + height * 2, "| ");
        } else {
            sprintf(printTreeStruct->padding[row] + height * 2, "\\-");
        }
        _fill(node->nodes[counter], printTreeStruct, height + 1, row + 1);
        row += nodesCount;
        counter++;
    }
}

void fill(Tree* tree, struct printTreeStruct* printTreeStruct) {
    _fill(tree->root, printTreeStruct, 0, 0);
}

int main(int argc, char** argv) {
    Tree* tree = newTree();
    makeTestTree(tree);
    struct printTreeStruct* exampleTreePrintStruct = initPrint(tree);
    fill(tree, exampleTreePrintStruct);
    printTreeStruct(exampleTreePrintStruct);
    freeTreePrintStruct(exampleTreePrintStruct);
    freeTree(tree);
    return 0;
}
