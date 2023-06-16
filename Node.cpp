#include "Node.h"

void Node::setLeftChild(uint l){ 
    leftChild = l; 
}

void Node::setRightChild(uint r){ 
    rightChild = r;
}

void Node::setParent(uint p){ 
    parent = p;
}

void Node::rotate(){
    uint temp = width;
    width = height;
    height = temp;
}

void Node::resize(uint w, uint h){
    width = w;
    height = h;
}
