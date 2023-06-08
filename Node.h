

#ifndef NODE_H
#define NODE_H

#include <string>
#include <stdio>

typedef enum BlockType{ 
    SOFT_BLOCK, 
    HARD_BLOCK 
} BLOCKTYPE;

class Node{
    BLOCKTYPE blocktype;
    std::string blockname;
    
    unsigned short block_width, block_height;
    int block_x, block_y; 
    
    double block_aspectratio;

    Node ();

};



#endif /*NODE_H*/