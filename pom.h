#ifndef POM_H
#define POM_H

#include <iostream>
#include <algorithm>
#include <limits>

/**
 * POM Tree (Partially Ordered Maximum Tree)
 * An augmented interval tree that maintains:
 * - sum: sum of all values in subtree
 * - maxpref: maximum prefix sum in subtree
 * - argmax: index of element achieving maxpref
 * 
 * Supports O(log n) operations for:
 * - interval insertion/deletion
 * - finding maximum prefix sum in any interval
 */

enum POMColor { POM_RED, POM_BLACK };

struct Interval {
    int start;
    int end;
    int value;
    
    Interval() : start(0), end(0), value(0) {}
    Interval(int s, int e, int v) : start(s), end(e), value(v) {}
    
    bool operator<(const Interval& other) const {
        return start < other.start;
    }
};

struct AugmentedData {
    long long sum;        // Sum of all values in subtree
    long long maxpref;    // Maximum prefix sum in subtree
    int argmax;           // Position achieving maxpref
    
    AugmentedData() : sum(0), maxpref(LLONG_MIN), argmax(-1) {}
    AugmentedData(long long s, long long mp, int am) 
        : sum(s), maxpref(mp), argmax(am) {}
};

struct POMNode {
    Interval interval;
    POMColor color;
    AugmentedData data;
    POMNode* left;
    POMNode* right;
    POMNode* parent;
    
    POMNode(Interval iv) : interval(iv), color(POM_RED), 
                           left(nullptr), right(nullptr), parent(nullptr) {
        data.sum = iv.value;
        data.maxpref = iv.value;
        data.argmax = iv.start;
    }
};

class POMTree {
private:
    POMNode* root;
    POMNode* nil;
    
    void leftRotate(POMNode* x) {
        POMNode* y = x->right;
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
        
        updateAugmentedData(x);
        updateAugmentedData(y);
    }
    
    void rightRotate(POMNode* y) {
        POMNode* x = y->left;
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
        
        updateAugmentedData(y);
        updateAugmentedData(x);
    }
    
    void updateAugmentedData(POMNode* node) {
        if (node == nil) return;
        
        // Sum: left.sum + node.value + right.sum
        node->data.sum = node->interval.value;
        if (node->left != nil) {
            node->data.sum += node->left->data.sum;
        }
        if (node->right != nil) {
            node->data.sum += node->right->data.sum;
        }
        
        // MaxPref computation
        // Three cases:
        // 1. Maxpref is in left subtree
        // 2. Maxpref includes current node (left.sum + node.value)
        // 3. Maxpref extends to right subtree
        
        long long leftSum = (node->left != nil) ? node->left->data.sum : 0;
        long long leftMaxPref = (node->left != nil) ? node->left->data.maxpref : LLONG_MIN;
        long long rightMaxPref = (node->right != nil) ? node->right->data.maxpref : LLONG_MIN;
        
        // Case 1: maxpref is entirely in left subtree
        node->data.maxpref = leftMaxPref;
        node->data.argmax = (node->left != nil) ? node->left->data.argmax : -1;
        
        // Case 2: maxpref includes current node
        long long prefixAtNode = leftSum + node->interval.value;
        if (prefixAtNode > node->data.maxpref) {
            node->data.maxpref = prefixAtNode;
            node->data.argmax = node->interval.start;
        }
        
        // Case 3: maxpref extends into right subtree
        if (node->right != nil && prefixAtNode + rightMaxPref > node->data.maxpref) {
            node->data.maxpref = prefixAtNode + rightMaxPref;
            node->data.argmax = node->right->data.argmax;
        }
    }
    
    void insertFixup(POMNode* z) {
        while (z->parent->color == POM_RED) {
            if (z->parent == z->parent->parent->left) {
                POMNode* y = z->parent->parent->right;
                if (y->color == POM_RED) {
                    z->parent->color = POM_BLACK;
                    y->color = POM_BLACK;
                    z->parent->parent->color = POM_RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = POM_BLACK;
                    z->parent->parent->color = POM_RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                POMNode* y = z->parent->parent->left;
                if (y->color == POM_RED) {
                    z->parent->color = POM_BLACK;
                    y->color = POM_BLACK;
                    z->parent->parent->color = POM_RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = POM_BLACK;
                    z->parent->parent->color = POM_RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = POM_BLACK;
    }
    
    void transplant(POMNode* u, POMNode* v) {
        if (u->parent == nil) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }
    
    POMNode* minimum(POMNode* x) {
        while (x->left != nil) {
            x = x->left;
        }
        return x;
    }
    
    void deleteFixup(POMNode* x) {
        while (x != root && x->color == POM_BLACK) {
            if (x == x->parent->left) {
                POMNode* w = x->parent->right;
                if (w->color == POM_RED) {
                    w->color = POM_BLACK;
                    x->parent->color = POM_RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == POM_BLACK && w->right->color == POM_BLACK) {
                    w->color = POM_RED;
                    x = x->parent;
                } else {
                    if (w->right->color == POM_BLACK) {
                        w->left->color = POM_BLACK;
                        w->color = POM_RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = POM_BLACK;
                    w->right->color = POM_BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                POMNode* w = x->parent->left;
                if (w->color == POM_RED) {
                    w->color = POM_BLACK;
                    x->parent->color = POM_RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == POM_BLACK && w->left->color == POM_BLACK) {
                    w->color = POM_RED;
                    x = x->parent;
                } else {
                    if (w->left->color == POM_BLACK) {
                        w->right->color = POM_BLACK;
                        w->color = POM_RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = POM_BLACK;
                    w->left->color = POM_BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = POM_BLACK;
    }
    
    POMNode* search(POMNode* x, const Interval& interval) {
        while (x != nil && !(interval.start == x->interval.start && 
                             interval.end == x->interval.end)) {
            if (interval < x->interval) {
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return x;
    }
    
    void destroyTree(POMNode* node) {
        if (node != nil) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }
    
    void updateAncestors(POMNode* node) {
        while (node != nil) {
            updateAugmentedData(node);
            node = node->parent;
        }
    }
    
public:
    POMTree() {
        nil = new POMNode(Interval());
        nil->color = POM_BLACK;
        nil->left = nil->right = nil->parent = nil;
        nil->data = AugmentedData();
        root = nil;
    }
    
    ~POMTree() {
        destroyTree(root);
        delete nil;
    }
    
    void insert(Interval interval) {
        POMNode* z = new POMNode(interval);
        z->left = z->right = nil;
        
        POMNode* y = nil;
        POMNode* x = root;
        
        while (x != nil) {
            y = x;
            if (z->interval < x->interval) {
                x = x->left;
            } else {
                x = x->right;
            }
        }
        
        z->parent = y;
        
        if (y == nil) {
            root = z;
        } else if (z->interval < y->interval) {
            y->left = z;
        } else {
            y->right = z;
        }
        
        z->color = POM_RED;
        insertFixup(z);
        updateAncestors(z);
    }
    
    void remove(Interval interval) {
        POMNode* z = search(root, interval);
        if (z == nil) return;
        
        POMNode* updateStart = z->parent;
        
        POMNode* y = z;
        POMNode* x;
        POMColor yOriginalColor = y->color;
        
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
            updateStart = y->parent;
            
            if (y->parent == z) {
                x->parent = y;
                updateStart = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        
        delete z;
        
        if (yOriginalColor == POM_BLACK) {
            deleteFixup(x);
        }
        
        updateAncestors(updateStart);
    }
    
    // Find maximum prefix sum and its position
    AugmentedData findPOM() {
        if (root == nil) {
            return AugmentedData();
        }
        return root->data;
    }
    
    long long getSum() {
        if (root == nil) return 0;
        return root->data.sum;
    }
    
    bool empty() {
        return root == nil;
    }
};

#endif // POM_H

