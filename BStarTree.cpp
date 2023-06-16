#include <vector>
#include <utility>
#include <list>
#include <climits>
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <iomanip>

#include "Node.h"
#include "BStarTree.h"
#include "Block.h"

BStarTree::BStarTree(){
    this->soft_block_num = 0;
    this->hard_block_num = 0;
    this->total_block_num = 0;
    this->isRendered = false;
    this->isLegal = false;
    this->boundingBoxMaxX = 0;
    this->boundingBoxMaxY = 0;

    std::random_device seed;
    this->rng = std::default_random_engine(seed());
    // this->rng = std::default_random_engine(8763);
    this->dist1 = std::uniform_int_distribution<int>(0,1);
}

bool BStarTree::checkLegal(){
    return this->isRendered && this->isLegal;
}

void BStarTree::render(){
    if (this->isRendered) {
        return;
    }
    std::list<std::pair<int,int>> horiContour;
    horiContour.push_back(std::pair<int,int>(0,0));
    horiContour.push_back(std::pair<int,int>(INT_MAX,0));
    this->boundingBoxMaxX = 0;
    this->boundingBoxMaxY = 0;
    
    dfs(bTree[0].rightChild, 0, 0, horiContour);
    this->isRendered = true;
}

void BStarTree::dfs(int blockIndex, int parentx, int parenty, std::list<std::pair<int,int>>& horiContour){
    // for a block placement to be valid, the block must follow two rules 
    // 1. block must be placed in a L shaped corner (unless block's X is 0, in which case the block placed will always be legal)
    // 2. block's y+h must be larger than parent's y
    
    Block* currentBlock = this->blockList[blockIndex];
    Node& currentNode = this->bTree[blockIndex];
    int currentWidth = currentNode.width;
    int currentHeight = currentNode.height;

    std::list<std::pair<int, int>>::iterator leftPoint, leftPointPrev, rightPoint, temp;
    bool leftFound = false, rightFound = false;
    for (std::list<std::pair<int, int>>::iterator it = horiContour.begin() ; it != horiContour.end(); ++it){
        // from left to right, leftPoint is the last point whose x <= block's x, leftPointPrev is the point directly before leftPoint, right point is the first point whose x >= block's (w+x) 
        if ((*it).first > parentx && !leftFound){
            leftFound = true;
            leftPoint = it;
            --leftPoint;
            leftPointPrev = leftPoint;
            --leftPointPrev;
        }
        if ((*it).first >= parentx+currentWidth && !rightFound){
            rightFound = true;
            rightPoint = it;
        }
    }
    
    // find y coordinate of new block, which is the point with the largest y between leftPoint and rightPoint
    int newY = 0;
    std::list<std::pair<int, int>>::iterator pastTheEnd = rightPoint;
    ++pastTheEnd;
    for (std::list<std::pair<int, int>>::iterator it = leftPoint; it != pastTheEnd; ++it){
        if ((*it).second > newY){
            newY = (*it).second;
        }
    }

    // check condition one
    if (parentx != 0 && !((*leftPoint).first == (*leftPointPrev).first && parentx == (*leftPoint).first && (*leftPoint).second < (*leftPointPrev).second && newY < (*leftPointPrev).second)){
        this->isLegal = false;
    }

    // check condition two
    if (newY+currentHeight <= parenty){
        this->isLegal = false;
    }

    // erase all elements between the two points, insert 4 new points
    temp = leftPoint;
    ++temp;
    horiContour.erase(temp, rightPoint);
    horiContour.insert(rightPoint, std::pair<int,int>(parentx,leftPoint->second));
    horiContour.insert(rightPoint, std::pair<int,int>(parentx,newY+currentHeight));
    horiContour.insert(rightPoint, std::pair<int,int>(parentx+currentWidth,newY+currentHeight));
    horiContour.insert(rightPoint, std::pair<int,int>(parentx+currentWidth,rightPoint->second));

    // "clean" contour, clearing sequences with 3 or more points with equal x coordinates
    int currentx = -1;
    int sameCounter = 0;
    std::list<std::pair<int, int>>::iterator firstDupe, lastDupe;
    for (std::list<std::pair<int, int>>::iterator it = horiContour.begin() ; it != horiContour.end(); ++it){
        if (it->first != currentx){
            if (sameCounter >= 3){
                // clear intermediate points
                ++firstDupe;
                horiContour.erase(firstDupe, lastDupe);
            }

            sameCounter = 1;
            currentx = it->first;
            firstDupe = it;
        }
        else {
            ++sameCounter;
            lastDupe = it;
        }
    }

    // store new x y coordinates and width height in block
    currentBlock->block_x = parentx;
    currentBlock->block_y = newY;
    currentBlock->width = currentWidth;
    currentBlock->height = currentHeight;

    if (currentBlock->block_x + currentWidth > boundingBoxMaxX){
        boundingBoxMaxX = currentBlock->block_x + currentWidth;
    }
    if (currentBlock->block_y + currentHeight > boundingBoxMaxY){
        boundingBoxMaxY = currentBlock->block_y + currentHeight;
    }

    if (currentNode.leftChild != 0){
        dfs(currentNode.leftChild, parentx+currentWidth, newY, horiContour);
    }

    if (currentNode.rightChild != 0){
        dfs(currentNode.rightChild, parentx, newY, horiContour);
    }

    return;
}

void BStarTree::init(std::vector<Block*> blockVector){

    this->bTree.push_back(Node()); // dummy node, represents head of btree
    this->blockList.push_back(NULL);
    for (int i = 1; i <= blockVector.size(); i++){
        Block* currentBlockPointer = blockVector[i-1];

        if (currentBlockPointer->blocktype == SOFT_BLOCK){
            this->soft_block_num++;
        }
        else { 
            this->hard_block_num++;
        }
        this->total_block_num++;

        this->blockList.push_back(currentBlockPointer);
        uint newParent = i/2;
        if (i == newParent*2){
            // is a left child of parent node
            this->bTree[newParent].setLeftChild(i);
        }
        else {
            // is a right child of parent node
            this->bTree[newParent].setRightChild(i);
        }

        this->bTree.push_back(Node(0, 0, newParent, currentBlockPointer->width, currentBlockPointer->height));
        this->id2BlockListIndex.insert(std::pair<int,int>(currentBlockPointer->id, i));
    }
    this->isRendered = false;

    // bool isLegal = checkLegal();
    render();
}

int BStarTree::perturbRotateBlock(Block* blockPointer){
    int blockIndex = id2BlockListIndex[blockPointer->id];
    bTree[blockIndex].rotate();

    this->isRendered = false;
    return 1; // ?
}



/// @param toSquare true if want to make into a square shape, false if want to make into a noodle shape
/// @return returns 1 if block is successfully changed, 0 if block stays the same
int BStarTree::perturbResizeSoftBlock(Block* blockPointer, bool toSquare){
    int blockIndex = id2BlockListIndex[blockPointer->id];
    Node& currentNode = bTree[blockIndex];
    int area = blockList[blockIndex]->area;
    int smallerValue = currentNode.width > currentNode.height ? currentNode.height : currentNode.width;
    int largerValue = currentNode.width > currentNode.height ? currentNode.width : currentNode.height;
    int newSmallValue = smallerValue, newLargeValue = largerValue;
    
    if (toSquare){
        for (int i = smallerValue + 1; smallerValue * smallerValue <= area; i++){
            if (area % i == 0){
                newSmallValue = i;
                newLargeValue = area / i;
                break;
            }
        } 
    }
    else {
        for (int i = smallerValue - 1; smallerValue > 0; i--){
            if (area % i == 0){
                newSmallValue = i;
                newLargeValue = area / i;
                break;
            }
        } 
    }

    if (smallerValue == newSmallValue){
        return 0;
    }

    currentNode.width = currentNode.width > currentNode.height ? newLargeValue : newSmallValue;
    currentNode.height = currentNode.width > currentNode.height ? newSmallValue : newLargeValue;
    this->isRendered = false;
    return 1;
}

int BStarTree::perturbMoveBlock(Block* move_node, Block* to_parent_node){
    int moveNodeIndex = id2BlockListIndex[move_node->id];
    int destinationNodeIndex = id2BlockListIndex[to_parent_node->id];
    if (moveNodeIndex == destinationNodeIndex){
        return 0;
    }
    removeFromTree(moveNodeIndex);
    insertNode(moveNodeIndex, destinationNodeIndex);

    this->isRendered = false;
    return 1;
}

void BStarTree::removeFromTree(int removeIndex){
    int leftOrRight = dist1(rng);
    // randomly choose left or right child to "bubble up" to original node 1 position
    if (bTree[removeIndex].leftChild != 0 && (leftOrRight == 0 || bTree[removeIndex].rightChild == 0)){
        //left
        int leftChildIndex = bTree[removeIndex].leftChild;
        int parentIndex = bTree[removeIndex].parent;
        recursiveBubbleUpNode(leftChildIndex, 0);
        bTree[leftChildIndex].setParent(parentIndex); 
        if (bTree[parentIndex].leftChild == removeIndex){
            bTree[parentIndex].setLeftChild(leftChildIndex);
        }
        else {
            bTree[parentIndex].setRightChild(leftChildIndex);
        }
    }
    else if (bTree[removeIndex].rightChild != 0 && (leftOrRight == 1 || bTree[removeIndex].leftChild == 0)){
        //right 
        int rightChildIndex = bTree[removeIndex].rightChild;
        int parentIndex = bTree[removeIndex].parent;
        recursiveBubbleUpNode(rightChildIndex, 1);
        bTree[rightChildIndex].setParent(parentIndex); 
        if (bTree[parentIndex].leftChild == removeIndex){
            bTree[parentIndex].setLeftChild(rightChildIndex);
        }
        else {
            bTree[parentIndex].setRightChild(rightChildIndex);
        }
    }
    else {
        // childless
        int parentIndex = bTree[removeIndex].parent;
        if (bTree[parentIndex].leftChild == removeIndex){
            bTree[parentIndex].setLeftChild(0);
        }
        else {
            bTree[parentIndex].setRightChild(0);
        }
    }
}

void BStarTree::recursiveBubbleUpNode(int blockIndex, int isWhichChild){
    // update pointers of the bottommost node first, call recursive function on random children
    int leftOrRight = dist1(rng);
    int child = 0;
    if (bTree[blockIndex].leftChild != 0 && (leftOrRight == 0 || bTree[blockIndex].rightChild == 0)){
        // left
        recursiveBubbleUpNode(bTree[blockIndex].leftChild, 0);
        child = bTree[blockIndex].leftChild;
    }
    else if (bTree[blockIndex].rightChild != 0 && (leftOrRight == 1 || bTree[blockIndex].leftChild == 0)){
        //right
        recursiveBubbleUpNode(bTree[blockIndex].rightChild, 1);
        child = bTree[blockIndex].rightChild;
    }

    if (isWhichChild == 0){
        // this node is a left child
        int parentIndex = bTree[blockIndex].parent;
        int siblingIndex = bTree[parentIndex].rightChild;
        bTree[blockIndex].setRightChild(siblingIndex);
        if (bTree[blockIndex].rightChild != 0){
            bTree[siblingIndex].setParent(blockIndex);
        }
        bTree[blockIndex].setLeftChild(child);
    }
    else {
        // this node is a right child
        int parentIndex = bTree[blockIndex].parent;
        int siblingIndex = bTree[parentIndex].leftChild;
        bTree[blockIndex].setLeftChild(siblingIndex);
        if (bTree[blockIndex].leftChild != 0){
            bTree[siblingIndex].setParent(blockIndex);
        }
        bTree[blockIndex].setRightChild(child);
    }
}

void BStarTree::insertNode(int moveIndex, int destinationIndex){
    int leftOrRight = dist1(rng);
    int anotherLeftOrRight = dist1(rng);
    if (leftOrRight == 0){
        // node1 will be inserted in left child of node 2
        if (anotherLeftOrRight == 0){
            // original left child of node 2 will become left child of node1
            if (bTree[destinationIndex].leftChild != 0){
                int leftChildIndex = bTree[destinationIndex].leftChild;
                bTree[leftChildIndex].setParent(moveIndex);
            }
            bTree[moveIndex].setLeftChild(bTree[destinationIndex].leftChild);
            bTree[moveIndex].setRightChild(0);
            bTree[moveIndex].setParent(destinationIndex);
            bTree[destinationIndex].setLeftChild(moveIndex);
        }
        else {
            // original left child of node 2 will become right child of node1            
            if (bTree[destinationIndex].leftChild != 0){
                int leftChildIndex = bTree[destinationIndex].leftChild;
                bTree[leftChildIndex].setParent(moveIndex);
            }
            bTree[moveIndex].setLeftChild(0);
            bTree[moveIndex].setRightChild(bTree[destinationIndex].leftChild);
            bTree[moveIndex].setParent(destinationIndex);
            bTree[destinationIndex].setLeftChild(moveIndex);
        }
    }
    else {
        // node 1 will be inserted in right child of node 2
        if (anotherLeftOrRight == 0){
            // original right child of node 2 will become left child of node1         
            if (bTree[destinationIndex].rightChild != 0){
                int rightChildIndex = bTree[destinationIndex].rightChild;
                bTree[rightChildIndex].setParent(moveIndex);
            }
            bTree[moveIndex].setLeftChild(bTree[destinationIndex].rightChild);
            bTree[moveIndex].setRightChild(0);
            bTree[moveIndex].setParent(destinationIndex);
            bTree[destinationIndex].setRightChild(moveIndex);   
        }
        else {
            // original right child of node 2 will become right child of node1
            if (bTree[destinationIndex].rightChild != 0){
                int rightChildIndex = bTree[destinationIndex].rightChild;
                bTree[rightChildIndex].setParent(moveIndex);
            }
            bTree[moveIndex].setLeftChild(0);
            bTree[moveIndex].setRightChild(bTree[destinationIndex].rightChild);
            bTree[moveIndex].setParent(destinationIndex);
            bTree[destinationIndex].setRightChild(moveIndex);   
        }
    }
}

int BStarTree::perturbSwapNode(Block* swap_a, Block* swap_b){
    int node1Index = id2BlockListIndex[swap_a->id];
    int node2Index = id2BlockListIndex[swap_b->id];
    int right1, right2, left1, left2, parent1, parent2;
    right1 = bTree[node1Index].rightChild;
    left1 = bTree[node1Index].leftChild;
    parent1 = bTree[node1Index].parent;
    right2 = bTree[node2Index].rightChild;
    left2 = bTree[node2Index].leftChild;
    parent2 = bTree[node2Index].parent;


    if (bTree[parent1].leftChild == node1Index){
        bTree[parent1].setLeftChild(node2Index);
    }
    else {
        bTree[parent1].setRightChild(node2Index);
    }

    if (bTree[parent2].leftChild == node2Index){
        bTree[parent2].setLeftChild(node1Index);
    }
    else {
        bTree[parent2].setRightChild(node1Index);
    }
    

    if (left1 != 0){
        bTree[left1].setParent(node2Index);
    }
    if (left2 != 0){
        bTree[left2].setParent(node1Index);
    }
    if (right1 != 0){
        bTree[right1].setParent(node2Index);
    }
    if (right2 != 0){
        bTree[right2].setParent(node1Index);
    }

    bTree[node1Index].setParent(parent2);
    bTree[node1Index].setRightChild(right2);
    bTree[node1Index].setLeftChild(left2);
    bTree[node2Index].setParent(parent1);
    bTree[node2Index].setRightChild(right1);
    bTree[node2Index].setLeftChild(left1);
    // std::swap(bTree[node1Index], bTree[node2Index]);


    // if node1Index and node2Index were originally parent & children, some of their pointers will be pointing towards themselves, therefore this needs to be fixed 
    if (bTree[node1Index].parent == node1Index){
        bTree[node1Index].setParent(node2Index);
    }
    else if (bTree[node1Index].leftChild == node1Index){
        bTree[node1Index].setLeftChild(node2Index);
    }
    else if (bTree[node1Index].rightChild == node1Index){
        bTree[node1Index].setRightChild(node2Index);
    }
    
    if (bTree[node2Index].parent == node2Index){
        bTree[node2Index].setParent(node1Index);
    }
    else if (bTree[node2Index].leftChild == node2Index){
        bTree[node2Index].setLeftChild(node1Index);
    }
    else if (bTree[node2Index].rightChild == node2Index){
        bTree[node2Index].setRightChild(node1Index);
    }
    
    this->isRendered = false;
}

void BStarTree::boundingBox(int* width, int* height){
    *width = boundingBoxMaxX;
    *height = boundingBoxMaxY;
}

void BStarTree::saveBest(){
    auto first = bTree.begin();
    auto last = bTree.end();
    std::copy(first, last, bestBTree.begin());
}

void BStarTree::loadBest(){
    auto first = bestBTree.begin();
    auto last = bestBTree.end();
    std::copy(first, last, bTree.begin());
    this->isRendered = false;
}

void BStarTree::saveCurrent(){
    auto first = bTree.begin();
    auto last = bTree.end();
    std::copy(first, last, savedBTree.begin());
}

void BStarTree::loadPrevCurrent(){
    auto first = savedBTree.begin();
    auto last = savedBTree.end();
    std::copy(first, last, bTree.begin());
    this->isRendered = false;
}

void BStarTree::printTree(std::ostream& fout){
    fout << "Root node: " << bTree[0].rightChild << '\n';
    for (int i = 1; i <= total_block_num; i++){
        fout << "\nNode " << std::setw(3) << i << '\n'; 
        fout << "BlockID: " << std::setw(3) << blockList[i]->id << "  Block name: " << blockList[i]->blockname << '\n';
        fout << "l: " << std::setw(3) << bTree[i].leftChild << "  r: " << std::setw(3) << bTree[i].rightChild << "  p: " << std::setw(3) << bTree[i].parent << '\n';
    }

}

void BStarTree::printFloorplan(std::ostream& fout, int fixedOutlineWidth, int fixedOutlineHeight){
    int frameWidth = boundingBoxMaxX > fixedOutlineWidth ? boundingBoxMaxX+100 : fixedOutlineWidth+100;
    int frameHeight = boundingBoxMaxY > fixedOutlineHeight ? boundingBoxMaxY+100 : fixedOutlineHeight+100;
    fout << total_block_num+1 << '\n';
    fout << frameWidth << ' ' << frameHeight << '\n';
    fout << "-1 0 0 " << fixedOutlineWidth << ' ' << fixedOutlineHeight << '\n';
    for (int i = 1; i <= total_block_num; i++){
        fout << i+1 << ' ' <<  blockList[i]->block_x << ' ' <<  blockList[i]->block_y << ' ' <<  blockList[i]->width << ' ' <<  blockList[i]->height << '\n';
    }
}
