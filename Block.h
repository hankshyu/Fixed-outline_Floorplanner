

#ifndef BLOCK_H
#define BLOCK_H

#include <string>

typedef enum BlockType{ 
    SOFT_BLOCK, 
    HARD_BLOCK 
} BLOCKTYPE;

struct Block{
    BLOCKTYPE blocktype;
    std::string blockname;
    int id;
        
    int area;
    int block_x, block_y; 
    int width, height;
    Block (): area(0), block_x(0), block_y(0), id(0), width(0), height(0) {}
    Block(int id, BLOCKTYPE bt, std::string name, int area, int width = 0, int height = 0, int x = 0, int y = 0): blocktype(bt), blockname(name), area(area), block_x(x), block_y(y), id(id), width(width), height(height) {}
    void initBlock(int id, BLOCKTYPE bt, std::string name, int area, int initWidth, int initHeight);
};



#endif /*BLOCK_H*/