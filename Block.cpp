#include <string>
#include "Block.h"

void Block::initBlock(int id, BLOCKTYPE bt, std::string name, int area, int initWidth, int initHeight){
    this->blocktype = bt;
    this->blockname = name;
    this->area = area;
    this->id = id;
    this->width = initWidth;
    this->height = initHeight;
}