#ifndef BSTARTREE_H
#define BSTARTREE_H

#include <vector>
#include "Node.h"

class BStarTree{

private:
    bool checkLegal();
    add();
    removefromtree();
    int soft_module_num;
    Node [];
    int hard_block_num;
    
    int total_block_num;
    


public:
    BStarTree();
    
    insertNode(); // random binary tree?
    starup()



    int peturbRotateNnode(Node block);
    int peturbResizeSoftBlock(Node block);

    int peturbMoveNode(Node move_node, Node to_parent_node);

    int peturbSwapNode(Node swap_a, Node swap_b);



    void boundingBox(int *width, int*height);
    
    
};


#endif /*BSTARTREE_H*/