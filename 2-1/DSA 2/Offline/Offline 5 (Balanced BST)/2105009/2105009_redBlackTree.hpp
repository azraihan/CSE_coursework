#include <string>

#ifndef _2105009_REDBLACKTREE
#define _2105009_REDBLACKTREE

using namespace std;

#define RED 0
#define BLACK 1

template<typename keyType=int, typename valueType=string>
class Node{
    public:
    keyType key;
    valueType value;
    
    int color;

    Node* left;
    Node* right;
    Node* parent;   //extra pointer to parent

    Node(int color, Node* parent=NULL, keyType key=-1, valueType value="", Node* left=NULL, Node* right=NULL);
};


template<typename nodeType>
class redBlackTree{
    nodeType* nilNode;
    nodeType* root;

    int nodeNum;

    void leftRotate(nodeType* x);
    void rightRotate(nodeType* x);

    void insertFix(nodeType* newNode);
    void eraseFixUp(nodeType* x);

    void transplant(nodeType* u, nodeType* v);

    nodeType* findMin(nodeType* root);

    void printHelper(nodeType* root);
    void inOrder(nodeType* root);
    nodeType* findHelper(int key);

    public:

    redBlackTree();

    void insert(nodeType* newNodeVal);
    void erase(int key);
    
    bool find(int key);
    int size();
    bool empty();
    void iterate();
    void clear();
    void print();
};

#include "2105009_redBlackTree.cpp"

#endif