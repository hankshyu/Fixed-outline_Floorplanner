

#ifndef NODE_H
#define NODE_H

typedef unsigned int uint;

struct Node{
    uint leftChild;
    uint rightChild;
    uint parent;
    
    double aspectRatio; // w/h

    Node(): leftChild(0), rightChild(0), parent(0), aspectRatio(1.0){}
    Node(uint l, uint r, uint p, double a = 1.0): leftChild(l), rightChild(r), parent(p), aspectRatio(a){}

    void setLeftChild(uint l){ leftChild = l; }
    void setRightChild(uint r){ rightChild = r; }
    void setParent(uint p){ parent = p; }
    void rotate(){ aspectRatio = 1.0/aspectRatio; }
    void setAspectRatio(double a){ aspectRatio = a; }
};



#endif /*NODE_H*/