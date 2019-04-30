/*
                = 4
                ^
             /     \
            &        |
          /  \     /   \
        11   ^   13     7
            /  \
          19    ~
               /  \
             7    NULL
 */

#include <stdlib.h>
#include <stdio.h>

struct node {
    struct node *left;
    struct node *right;
    int value;
};

typedef struct ExprTree {
    struct node *root;
} ExprTree;

ExprTree *makeTestTree();
void freeExprTree(ExprTree*);
void freeExprTreeNode(struct node*);
unsigned int evalNode(struct node*);
unsigned int evalTree(ExprTree*);

struct node *newValueNode(int value) {
    struct node *ret = malloc(sizeof(struct node));
    ret->left = ret->right = NULL;
    ret->value = value;
    return ret;
}

struct node *newOpNode(int op, struct node *left, struct node *right) {
    struct node *ret = malloc(sizeof(struct node));
    ret->left = left;
    ret->right = right;
    ret->value = op;
    return ret;
}

ExprTree *makeTestTree() {
    ExprTree *ret = malloc(sizeof(ExprTree));
    ret->root = newOpNode('^',
                          newOpNode('&',
                                    newValueNode(11),
                                    newOpNode('^',
                                              newValueNode(19),
                                              newOpNode('~',
                                                        newValueNode(7),
                                                        NULL))),
                          newOpNode('|',
                                    newValueNode(13),
                                    newValueNode(7)));
    return ret;
}

void freeExprTreeNode(struct node *node) {
    if (node->left)
        freeExprTreeNode(node->left);
    if (node->right)
        freeExprTreeNode(node->right);
    free(node);
}

void freeExprTree(ExprTree *tree) {
    if (tree->root) {
        freeExprTreeNode(tree->root);
    }
    free(tree);
}

unsigned int evalNode(struct node *node) {
    if (!node->left && !node->right)
        return node->value;
    switch (node->value) {
        case '^':
            return evalNode(node->left) ^ evalNode(node->right);
            break;
        case '|':
            return evalNode(node->left) | evalNode(node->right);
            break;
        case '&':
            return evalNode(node->left) & evalNode(node->right);
            break;
        case '~': {
            if (!node->left)
                return ~evalNode(node->right);
            if (!node->right)
                return ~evalNode(node->left);
            break;
        }
        default:
            break;
    }
    return 0;
}

unsigned int evalTree(ExprTree *tree) {
    return evalNode(tree->root);
}

int main(int argc, char **argv) {
    ExprTree *tree = makeTestTree();
    unsigned int result = evalTree(tree);
    printf("result: %d\n", result);
    freeExprTree(tree);
    return 0;
}

