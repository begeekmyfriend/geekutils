/*
 * Copyright (C) 2015, Leo Ma <begeekmyfriend@gmail.com>
 */

#ifndef _RBTREE_H
#define _RBTREE_H

#define rbtree_red(_node)           ((_node)->color = 1)
#define rbtree_black(_node)         ((_node)->color = 0)
#define rbtree_is_red(_node)        ((_node)->color)
#define rbtree_is_black(_node)      (!rbtree_is_red(_node))
#define rbtree_copy_color(_n1, _n2) ((_n1)->color = (_n2)->color)

struct rbnode {
        struct rbnode *left;     /* left link */
        struct rbnode *right;    /* right link */
        struct rbnode *parent;   /* parent link */
        int           color;     /* red | black */
        int           key;       /* key for ordering */
        int           data;     /* opaque data */
};

struct rbtree {
        struct rbnode *root;     /* root node */
        struct rbnode *sentinel; /* nil node */
};

static void
rbtree_node_init(struct rbnode *node)
{
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->key = -1;
        node->data = -1;
        /* color is left uninitialized */
}

static void
rbtree_init(struct rbtree *tree, struct rbnode *node)
{
        rbtree_node_init(node);
        rbtree_black(node);
        tree->root = node;
        tree->sentinel = node;
}

static struct rbnode *
rbtree_node_min(struct rbnode *node, struct rbnode *sentinel)
{
        while (node->left != sentinel) {
                node = node->left;
        }
        return node;
}

static struct rbnode *
rbtree_node_max(struct rbnode *node, struct rbnode *sentinel)
{
        while (node->right != sentinel) {
                node = node->right;
        }
        return node;
}

static struct rbnode *
rbtree_prev(struct rbtree *tree, struct rbnode *node)
{
        struct rbnode *root = tree->root;
        struct rbnode *sentinel = tree->sentinel;
        struct rbnode *left, *parent;

        if (node == sentinel) {
                return NULL;
        }

        left = node->left;
        if (left != sentinel) {
                return rbtree_node_max(left, sentinel);
        }

        parent = node->parent;
        while (parent != root && node == parent->left) {
                node = parent;
                parent = node->parent;
        }

        return parent;
}

static struct rbnode *
rbtree_next(struct rbtree *tree, struct rbnode *node)
{
        struct rbnode *root = tree->root;
        struct rbnode *sentinel = tree->sentinel;
        struct rbnode *right, *parent;

        if (node == sentinel) {
                return NULL;
        }

        right = node->right;
        if (right != sentinel) {
                return rbtree_node_min(right, sentinel);
        }

        parent = node->parent;
        while (parent != root && node == parent->right) {
                node = parent;
                parent = node->parent;
        }

        return parent;
}

static struct rbnode *
rbtree_min(struct rbtree *tree)
{
        struct rbnode *node = tree->root;
        struct rbnode *sentinel = tree->sentinel;

        if (node == sentinel) {
                /* empty tree */
                return NULL;
        } else {
                return rbtree_node_min(node, sentinel);
        }
}

static struct rbnode *
rbtree_max(struct rbtree *tree)
{
        struct rbnode *node = tree->root;
        struct rbnode *sentinel = tree->sentinel;

        if (node == sentinel) {
                /* empty tree */
                return NULL;
        } else {
                return rbtree_node_max(node, sentinel);
        }
}

static struct rbnode *
rbtree_search(struct rbtree *tree, int key)
{
        struct rbnode *node = tree->root;
        struct rbnode *sentinel = tree->sentinel;

        while (node != sentinel) {
                if (key < node->key) {
                        node = node->left;
                } else if (key > node->key) {
                        node = node->right;
                } else {
                        return node;
                }
        }
        return NULL;
}

static void
rbtree_left_rotate(struct rbnode **root, struct rbnode *sentinel,
                   struct rbnode *node)
{
        struct rbnode *right = node->right;
        node->right = right->left;

        if (right->left != sentinel) {
                right->left->parent = node;
        }

        right->parent = node->parent;

        if (node == *root) {
                *root = right;
        } else if (node == node->parent->left) {
                node->parent->left = right;
        } else {
                node->parent->right = right;
        }

        right->left = node;
        node->parent = right;
}

static void
rbtree_right_rotate(struct rbnode **root, struct rbnode *sentinel,
                    struct rbnode *node)
{
        struct rbnode *left = node->left;
        node->left = left->right;

        if (left->right != sentinel) {
                left->right->parent = node;
        }

        left->parent = node->parent;

        if (node == *root) {
                *root = left;
        } else if (node == node->parent->right) {
                node->parent->right = left;
        } else {
                node->parent->left = left;
        }

        left->right = node;
        node->parent = left;
}

static void
rbtree_insert(struct rbtree *tree, struct rbnode *node)
{
        struct rbnode **root = &tree->root;
        struct rbnode *sentinel = tree->sentinel;
        struct rbnode *parent, *uncle, **p;

        /* empty tree */
        if (*root == sentinel) {
                node->parent = NULL;
                node->left = sentinel;
                node->right = sentinel;
                rbtree_black(node);
                *root = node;
                return;
        }

        /* a binary tree insert */
        parent = *root;
        for (;;) {
                p = (node->key < parent->key) ? &parent->left : &parent->right;
                if (*p == sentinel) {
                        break;
                }
                parent = *p;
        }
        *p = node;
        node->parent = parent;
        node->left = sentinel;
        node->right = sentinel;
        rbtree_red(node);

        /* re-balance tree */
        while (node != *root && rbtree_is_red(node->parent)) {

                if (node->parent == node->parent->parent->left) {
                        uncle = node->parent->parent->right;

                        if (rbtree_is_red(uncle)) {
                                /* no need to rotate */
                                rbtree_black(node->parent);
                                rbtree_black(uncle);
                                rbtree_red(node->parent->parent);
                                node = node->parent->parent;
                        } else {
                                if (node == node->parent->right) {
                                        /* inside insert left rotate */
                                        node = node->parent;
                                        rbtree_left_rotate(root, sentinel, node);
                                }
                                /* outside insert right rotate */
                                rbtree_black(node->parent);
                                rbtree_red(node->parent->parent);
                                rbtree_right_rotate(root, sentinel, node->parent->parent);
                        }
                } else {
                        uncle = node->parent->parent->left;

                        if (rbtree_is_red(uncle)) {
                                /* no need to rotate */
                                rbtree_black(node->parent);
                                rbtree_black(uncle);
                                rbtree_red(node->parent->parent);
                                node = node->parent->parent;
                        } else {
                                if (node == node->parent->left) {
                                        /* inside insert right rotate */
                                        node = node->parent;
                                        rbtree_right_rotate(root, sentinel, node);
                                }
                                /* outside insert left rotate */
                                rbtree_black(node->parent);
                                rbtree_red(node->parent->parent);
                                rbtree_left_rotate(root, sentinel, node->parent->parent);
                        }
                }
        }

        rbtree_black(*root);
}

static void
rbtree_delete(struct rbtree *tree, struct rbnode *node)
{
        int red;
        struct rbnode **root = &tree->root;
        struct rbnode *sentinel = tree->sentinel;
        struct rbnode *subst, *temp, *w;

        /* a binary tree delete */
        if (node == sentinel) {
                return;
        } else if (node->left == sentinel) {
                temp = node->right;
                subst = node;
        } else if (node->right == sentinel) {
                temp = node->left;
                subst = node;
        } else {
                subst = rbtree_node_min(node->right, sentinel);
                temp = subst->right;
        }

        if (subst == *root) {
                /* node == subst == *root */
                *root = temp;
                rbtree_black(temp);
                rbtree_node_init(node);
                return;
        }

        red = rbtree_is_red(subst);

        if (subst == subst->parent->left) {
                subst->parent->left = temp;
        } else {
                subst->parent->right = temp;
        }

        if (subst == node) {
                temp->parent = subst->parent;
        } else {
                if (subst->parent == node) {
                        temp->parent = subst;
                } else {
                        temp->parent = subst->parent;
                }

                subst->left = node->left;
                subst->right = node->right;
                subst->parent = node->parent;
                rbtree_copy_color(subst, node);

                if (node == *root) {
                        *root = subst;
                } else {
                        if (node == node->parent->left) {
                                node->parent->left = subst;
                        } else {
                                node->parent->right = subst;
                        }
                }

                if (subst->left != sentinel) {
                        subst->left->parent = subst;
                }

                if (subst->right != sentinel) {
                        subst->right->parent = subst;
                }
        }

        rbtree_node_init(node);

        if (red) {
                return;
        }

        /* a delete fixup */
        while (temp != *root && rbtree_is_black(temp)) {

                if (temp == temp->parent->left) {
                        w = temp->parent->right;

                        if (rbtree_is_red(w)) {
                                rbtree_black(w);
                                rbtree_red(temp->parent);
                                rbtree_left_rotate(root, sentinel, temp->parent);
                                w = temp->parent->right;
                        }

                        if (rbtree_is_black(w->left) && rbtree_is_black(w->right)) {
                                rbtree_red(w);
                                temp = temp->parent;
                        } else {
                                if (rbtree_is_black(w->right)) {
                                        rbtree_black(w->left);
                                        rbtree_red(w);
                                        rbtree_right_rotate(root, sentinel, w);
                                        w = temp->parent->right;
                                }

                                rbtree_copy_color(w, temp->parent);
                                rbtree_black(temp->parent);
                                rbtree_black(w->right);
                                rbtree_left_rotate(root, sentinel, temp->parent);
                                temp = *root;
                        }

                } else {
                        w = temp->parent->left;

                        if (rbtree_is_red(w)) {
                                rbtree_black(w);
                                rbtree_red(temp->parent);
                                rbtree_right_rotate(root, sentinel, temp->parent);
                                w = temp->parent->left;
                        }

                        if (rbtree_is_black(w->left) && rbtree_is_black(w->right)) {
                                rbtree_red(w);
                                temp = temp->parent;
                        } else {
                                if (rbtree_is_black(w->left)) {
                                        rbtree_black(w->right);
                                        rbtree_red(w);
                                        rbtree_left_rotate(root, sentinel, w);
                                        w = temp->parent->left;
                                }

                                rbtree_copy_color(w, temp->parent);
                                rbtree_black(temp->parent);
                                rbtree_black(w->left);
                                rbtree_right_rotate(root, sentinel, temp->parent);
                                temp = *root;
                        }
                }
        }

        rbtree_black(temp);
}

#endif  /* _RBTREE_H */
