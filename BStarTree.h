#ifndef BSTARTREE_H
#define BSTARTREE_H

#include <vector>
#include <string>
#include <map>
#include <random>
#include <utility>
#include <list>
#include "Node.h"
#include "Block.h"

typedef enum Direction {
    LEFT,
    RIGHT
} DIRECTION;


class Connection {
public:
    Block* first;
    Block* second;
    int num_nets;
};

class BStarTree {

private:
    bool checkLegal();
    void dfs(int blockIndex, int parentx, int parenty, std::list< std::pair<int, int> >& horiContour);
    void insertNode(int moveIndex, int destinationIndex, bool rightChildOfDest);
    void removeFromTree(int removeIndex, bool dontTouchPPM);
    void recursiveBubbleUpNode(int blockIndex, int isWhichChild, bool dontTouchPPM);
    void simpleDFS(int index, std::vector<int>& isTraversed);

    int soft_block_num;
    int hard_block_num;
    int total_block_num;
    int originX, originY;
    int boundingBoxMaxX, boundingBoxMaxY;
    bool isRendered;
    bool isLegal;

    std::vector<Node> bTree;
    std::vector<Block*> blockList;
    std::map<int, int> id2BlockListIndex;

    std::vector<Node> bestBTree;
    std::vector<Node> savedBTree;

    std::default_random_engine rng;
    std::uniform_int_distribution<int> dist1; // choose between 0 and 1

public:
    BStarTree();

    void init(std::vector<Block*> blockVector, int CHIP_WIDTH, int CHIP_HEIGHT);

    void noPPMInit(std::vector<Block*> blockVector, int CHIP_WIDTH, int CHIP_HEIGHT,
        int* modifiedChipWidth, int* modifiedChipHeight);

    int perturbRotateBlock(Block* blockPointer);

    int perturbResizeSoftBlock(Block* blockPointer, bool toSquare);

    int perturbMoveBlock(Block* move_node, Block* to_parent_node);

    int perturbSwapNode(Block* swap_a, Block* swap_b);

    void boundingBox(int* width, int* height);

    void saveBest();
    void loadBest();

    void saveCurrent();
    void loadPrevCurrent();

    void printTree(std::string filename);
    void printFloorplan(std::string filename, int fixedOutlineWidth, int fixedOutlineHeight, std::vector<Connection*>& connection_vector);

    void render();
    bool getisLegal() { return this->isLegal; }
    bool getisRendered() { return this->isRendered; }

    bool checkTreeValid();
};


#endif /*BSTARTREE_H*/