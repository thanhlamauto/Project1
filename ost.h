#ifndef OST_H
#define OST_H

#include <iostream>
#include <memory>
#include <functional>

/**
 * Order Statistic Tree (OST)
 * An augmented Red-Black Tree that maintains subtree sizes
 * Supports O(log n) operations for:
 * - insert, delete, search
 * - select (find k-th smallest element)
 * - rank (find position of element)
 */

enum Color { RED, BLACK };

template<typename T>
struct OSTNode {
    T key;
    Color color;
    int size;  // Size of subtree rooted at this node
    OSTNode* left;
    OSTNode* right;
    OSTNode* parent;
    
    OSTNode(T k) : key(k), color(RED), size(1), 
                   left(nullptr), right(nullptr), parent(nullptr) {}
};

template<typename T>
class OrderStatisticTree {
private:
    OSTNode<T>* root;
    OSTNode<T>* nil;  // Sentinel node
    
    void leftRotate(OSTNode<T>* x) {
        OSTNode<T>* y = x->right;
        x->right = y->left;
        
        if (y->left != nil) {
            y->left->parent = x;
        }
        
        y->parent = x->parent;
        
        if (x->parent == nil) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        
        y->left = x;
        x->parent = y;
        
        // Update sizes
        y->size = x->size;
        x->size = getSize(x->left) + getSize(x->right) + 1;
    }
    
    void rightRotate(OSTNode<T>* y) {
        OSTNode<T>* x = y->left;
        y->left = x->right;
        
        if (x->right != nil) {
            x->right->parent = y;
        }
        
        x->parent = y->parent;
        
        if (y->parent == nil) {
            root = x;
        } else if (y == y->parent->right) {
            y->parent->right = x;
        } else {
            y->parent->left = x;
        }
        
        x->right = y;
        y->parent = x;
        
        // Update sizes
        x->size = y->size;
        y->size = getSize(y->left) + getSize(y->right) + 1;
    }
    
    void insertFixup(OSTNode<T>* z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                OSTNode<T>* y = z->parent->parent->right;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                OSTNode<T>* y = z->parent->parent->left;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }
    
    void transplant(OSTNode<T>* u, OSTNode<T>* v) {
        if (u->parent == nil) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }
    
    OSTNode<T>* minimum(OSTNode<T>* x) {
        while (x->left != nil) {
            x = x->left;
        }
        return x;
    }
    
    void deleteFixup(OSTNode<T>* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                OSTNode<T>* w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                OSTNode<T>* w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }
    
    int getSize(OSTNode<T>* node) {
        return (node == nil) ? 0 : node->size;
    }
    
    void updateSize(OSTNode<T>* node) {
        if (node != nil) {
            node->size = getSize(node->left) + getSize(node->right) + 1;
        }
    }
    
    void destroyTree(OSTNode<T>* node) {
        if (node != nil) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }
    
public:
    OrderStatisticTree() {
        nil = new OSTNode<T>(T());
        nil->color = BLACK;
        nil->size = 0;
        nil->left = nil->right = nil->parent = nil;
        root = nil;
    }
    
    ~OrderStatisticTree() {
        destroyTree(root);
        delete nil;
    }
    
    void insert(T key) {
        OSTNode<T>* z = new OSTNode<T>(key);
        z->left = z->right = nil;
        
        OSTNode<T>* y = nil;
        OSTNode<T>* x = root;
        
        while (x != nil) {
            y = x;
            x->size++;  // Increment size along the path
            if (z->key < x->key) {
                x = x->left;
            } else {
                x = x->right;
            }
        }
        
        z->parent = y;
        
        if (y == nil) {
            root = z;
        } else if (z->key < y->key) {
            y->left = z;
        } else {
            y->right = z;
        }
        
        z->color = RED;
        insertFixup(z);
    }
    
    void remove(T key) {
        OSTNode<T>* z = search(root, key);
        if (z == nil) return;
        
        // Decrement sizes along path from z to root
        OSTNode<T>* p = z;
        while (p != nil) {
            p->size--;
            p = p->parent;
        }
        
        OSTNode<T>* y = z;
        OSTNode<T>* x;
        Color yOriginalColor = y->color;
        
        if (z->left == nil) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == nil) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            
            if (y->parent == z) {
                x->parent = y;
            } else {
                // Decrement sizes from y to z
                OSTNode<T>* p = y->parent;
                while (p != z) {
                    p->size--;
                    p = p->parent;
                }
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
            y->size = getSize(y->left) + getSize(y->right) + 1;
        }
        
        delete z;
        
        if (yOriginalColor == BLACK) {
            deleteFixup(x);
        }
    }
    
    OSTNode<T>* search(OSTNode<T>* x, T key) {
        while (x != nil && key != x->key) {
            if (key < x->key) {
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return x;
    }
    
    // Find k-th smallest element (1-indexed)
    T select(int k) {
        OSTNode<T>* node = selectNode(root, k);
        if (node == nil) {
            throw std::out_of_range("Index out of range");
        }
        return node->key;
    }
    
    OSTNode<T>* selectNode(OSTNode<T>* x, int k) {
        if (x == nil) return nil;
        
        int r = getSize(x->left) + 1;
        if (k == r) {
            return x;
        } else if (k < r) {
            return selectNode(x->left, k);
        } else {
            return selectNode(x->right, k - r);
        }
    }
    
    // Find rank (position) of element (1-indexed)
    int rank(T key) {
        OSTNode<T>* x = search(root, key);
        if (x == nil) return -1;
        
        int r = getSize(x->left) + 1;
        OSTNode<T>* y = x;
        
        while (y != root) {
            if (y == y->parent->right) {
                r += getSize(y->parent->left) + 1;
            }
            y = y->parent;
        }
        
        return r;
    }
    
    int size() {
        return getSize(root);
    }
    
    bool empty() {
        return root == nil;
    }
};

#endif // OST_H

