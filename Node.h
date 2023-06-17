#ifndef NODE_H
#define NODE_H

typedef unsigned int uint;

struct Node{
    uint leftChild;
    uint rightChild;
    uint parent;
    
    uint width;
    uint height;

    Node(): leftChild(0), rightChild(0), parent(0), width(0), height(0){}
    Node(uint l, uint r, uint p, uint w, uint h): leftChild(l), rightChild(r), parent(p), width(w), height(h){}

    void setLeftChild(uint l);
    void setRightChild(uint r);
    void setParent(uint p);
    void rotate();
    void resize(uint w, uint h);
};


#endif /*NODE_H*/