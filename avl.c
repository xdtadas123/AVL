#include "avl.h"
#include <stdlib.h>

struct Node {
    int key;
    struct Node* left;
    struct Node* right;
    int height;
};

struct AvlTree {
    struct Node* root;
};

static int get_height(const struct Node* n)
{
    return (n == NULL) ? 0 : n->height;
}

static int max_int(int a, int b)
{
    return (a > b) ? a : b;
}

static int get_balance_factor(const struct Node* n)
{
    if (n == NULL) {
        return 0;
    }
    return get_height(n->left) - get_height(n->right);
}

static struct Node* min_value_node(struct Node* node)
{
    struct Node* cur = node;
    while (cur != NULL && cur->left != NULL) {
        cur = cur->left;
    }
    return cur;
}

static struct Node* create_node(int key)
{
    struct Node* node = (struct Node*)malloc(sizeof(*node));
    if (node == NULL) {
        return NULL;
    }
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

static struct Node* right_rotate(struct Node* y)
{
    struct Node* x = y->left;
    struct Node* t2 = x->right;

    x->right = y;
    y->left = t2;

    y->height = max_int(get_height(y->left), get_height(y->right)) + 1;
    x->height = max_int(get_height(x->left), get_height(x->right)) + 1;

    return x;
}

static struct Node* left_rotate(struct Node* x)
{
    struct Node* y = x->right;
    struct Node* t2 = y->left;

    y->left = x;
    x->right = t2;

    x->height = max_int(get_height(x->left), get_height(x->right)) + 1;
    y->height = max_int(get_height(y->left), get_height(y->right)) + 1;

    return y;
}

static struct Node* insert_node(struct Node* node, int key, int* inserted, int* oom)
{
    int balance;

    if (*oom) {
        return node;
    }

    if (node == NULL) {
        struct Node* nn = create_node(key);
        if (nn == NULL) {
            *oom = 1;
            return NULL;
        }
        *inserted = 1;
        return nn;
    }

    if (key < node->key) {
        struct Node* new_left = insert_node(node->left, key, inserted, oom);
        if (*oom) {
            return node;
        }
        node->left = new_left;
    } else if (key > node->key) {
        struct Node* new_right = insert_node(node->right, key, inserted, oom);
        if (*oom) {
            return node;
        }
        node->right = new_right;
    } else {
        return node;
    }

    node->height = 1 + max_int(get_height(node->left), get_height(node->right));
    balance = get_balance_factor(node);

    //LL
    if (balance > 1 && key < node->left->key) {
        return right_rotate(node);
    }

    //RR
    if (balance < -1 && key > node->right->key) {
        return left_rotate(node);
    }

    //LR
    if (balance > 1 && key > node->left->key) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }

    //RL
    if (balance < -1 && key < node->right->key) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

static struct Node* remove_node(struct Node* node, int key, int* removed)
{
    int balance;

    if (node == NULL) {
        return NULL;
    }

    if (key < node->key) {
        node->left = remove_node(node->left, key, removed);
    } else if (key > node->key) {
        node->right = remove_node(node->right, key, removed);
    } else {
        *removed = 1;
        if (node->left == NULL || node->right == NULL) {
            struct Node* child = (node->left != NULL) ? node->left : node->right;

            free(node);
            return child;
        }

        {
            struct Node* succ = min_value_node(node->right);
            node->key = succ->key;
            node->right = remove_node(node->right, succ->key, removed);
        }
    }

    if (node == NULL) {
        return NULL;
    }

    node->height = 1 + max_int(get_height(node->left), get_height(node->right));
    balance = get_balance_factor(node);

    //LL
    if (balance > 1 && get_balance_factor(node->left) >= 0) {
        return right_rotate(node);
    }

    //LR
    if (balance > 1 && get_balance_factor(node->left) < 0) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }

    //RR
    if (balance < -1 && get_balance_factor(node->right) <= 0) {
        return left_rotate(node);
    }

    //RL
    if (balance < -1 && get_balance_factor(node->right) > 0) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

static void destroy_node(struct Node* node)
{
    if (node == NULL) {
        return;
    }
    destroy_node(node->left);
    destroy_node(node->right);
    free(node);
}

static int contains_node(const struct Node* node, int key)
{
    while (node != NULL) {
        if (key < node->key) {
            node = node->left;
        } else if (key > node->key) {
            node = node->right;
        } else {
            return 1;
        }
    }
    return 0;
}

static void inorder_node(const struct Node* node, AvlVisitFn visit, void* user_data)
{
    if (node == NULL) {
        return;
    }
    inorder_node(node->left, visit, user_data);
    visit(node->key, user_data);
    inorder_node(node->right, visit, user_data);
}

/* ===== API ===== */

int avl_create(AvlTree** out_tree)
{
    AvlTree* t;

    if (out_tree == NULL) {
        return 0;
    }

    t = (AvlTree*)malloc(sizeof(*t));
    if (t == NULL) {
        *out_tree = NULL;
        return 0;
    }

    t->root = NULL;
    *out_tree = t;
    return 1;
}

void avl_destroy(AvlTree* tree)
{
    if (tree == NULL) {
        return;
    }
    destroy_node(tree->root);
    free(tree);
}

int avl_insert(AvlTree* tree, int key)
{
    int inserted = 0;
    int oom = 0;

    if (tree == NULL) {
        return -1;
    }

    tree->root = insert_node(tree->root, key, &inserted, &oom);

    if (oom) {
        return -1;
    }
    return inserted ? 1 : 0;
}

int avl_remove(AvlTree* tree, int key)
{
    int removed = 0;

    if (tree == NULL) {
        return -1;
    }

    tree->root = remove_node(tree->root, key, &removed);
    return removed ? 1 : 0;
}

int avl_contains(const AvlTree* tree, int key)
{
    if (tree == NULL) {
        return 0;
    }
    return contains_node(tree->root, key);
}

void avl_inorder(const AvlTree* tree, AvlVisitFn visit, void* user_data)
{
    if (tree == NULL || visit == NULL) {
        return;
    }
    inorder_node(tree->root, visit, user_data);
}
