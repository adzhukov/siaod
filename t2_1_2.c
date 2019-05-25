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
    enum types {
        valueNode,
        opNode,
        variableNode
    } type;
    struct node *left;
    struct node *right;
    union {
        double value;
        char op;
        int var;
    } value;
};

typedef struct ExprTree {
    struct node *root;
} ExprTree;

struct vars {
    double value;
    unsigned int valid;
};

struct vars vars[256];

struct node *newValueNode(double);
struct node *newOpNode(char, struct node*, struct node*);
struct node *newVarNode(char);
ExprTree *makeTestTree();
void freeExprTree(ExprTree*);
void freeExprTreeNode(struct node*);
double evalNode(struct node*);
double evalTree(ExprTree*);

struct node *newValueNode(double value) {
    struct node *ret = malloc(sizeof(struct node));
    ret->type = valueNode;
    ret->left = ret->right = NULL;
    ret->value.value = value;
    return ret;
}

struct node *newOpNode(char op, struct node *left, struct node *right) {
    struct node *ret = malloc(sizeof(struct node));
    ret->type = opNode;
    ret->left = left;
    ret->right = right;
    ret->value.op = op;
    return ret;
}

struct node *newVarNode(char name) {
    struct node *ret = malloc(sizeof(struct node));
    ret->type = variableNode;
    ret->left = ret->right = NULL;
    ret->value.var = name;
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
    if (node->type == valueNode)
        return node->value.value;
    else if (node->type == variableNode) {
        if (!vars[node->value.var].valid) {
            printf("enter variable %c: ", node->value.var);
            scanf("%lf", &(vars[node->value.var].value));
            vars[node->value.var].valid = 1;
        }
        return vars[node->value.var].value;
    }
    switch (node->value.op) {
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
    ExprTree *tree = makeTestTree();
    double result = evalTree(tree);
    printf("result: %lf\n", result);
    freeExprTree(tree);
    return 0;
}
