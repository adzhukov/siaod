/*
                   +
               /      \
              *         /
             /  \     /   \
            c    -   b      a
                /  \
              b     -
                   /  \
                  a    NULL
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct node {
    struct node *left;
    struct node *right;
    int value;
};

typedef struct ExprTree {
    struct node *root;
} ExprTree;

struct vars {
    unsigned int value;
    unsigned int valid;
};

struct vars vars[26];

struct node *newValueNode(int);
struct node *newOpNode(int, struct node*, struct node*);
struct node *newVarNode(char);
ExprTree *makeTestTree();
void freeExprTree(ExprTree*);
void freeExprTreeNode(struct node*);
double evalNode(struct node*);
double evalTree(ExprTree*);

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

struct node *newVarNode(char name) {
    struct node *ret = malloc(sizeof(struct node));
    ret->left = ret->right = NULL;
    ret->value = name;
    return ret;
}

ExprTree *makeTestTree() {
    ExprTree *ret = malloc(sizeof(ExprTree));
    ret->root = newOpNode('+',
                          newOpNode('*',
                                    newVarNode('c'),
                                    newOpNode('-',
                                              newVarNode('b'),
                                              newOpNode('-',
                                                        newVarNode('a'),
                                                        NULL))),
                          newOpNode('/',
                                    newVarNode('b'),
                                    newVarNode('a')));
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

double evalNode(struct node *node) {
    if (!node->left && !node->right) {
        if (node->value >= 'a' && node->value <= 'z') {
            if (!vars[node->value - 'a'].valid) {
                printf("enter variable %c: ", node->value);
                scanf("%d", &(vars[node->value - 'a'].value));
                vars[node->value - 'a'].valid = 1;
            }
            return vars[node->value - 'a'].value;
        }
        return node->value;
    }
    switch (node->value) {
        case '+':
            return evalNode(node->left) + evalNode(node->right);
            break;
        case '*':
            return evalNode(node->left) * evalNode(node->right);
            break;
        case '/':
            return evalNode(node->left) / evalNode(node->right);
            break;
        case '-': {
            if (!node->left)
                return -evalNode(node->right);
            if (!node->right)
                return -evalNode(node->left);
            return evalNode(node->left) - evalNode(node->right);
            break;
        }
        default:
            break;
    }
    return 0.0;
}

double evalTree(ExprTree *tree) {
    return evalNode(tree->root);
}

int main(int argc, char **argv) {
    memset(vars, 0, sizeof(struct vars) * 26);
    ExprTree *tree = makeTestTree();
    double result = evalTree(tree);
    printf("result: %lf\n", result);
    freeExprTree(tree);
    return 0;
}
