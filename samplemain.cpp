#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Block.h"
#include "BStarTree.h"




int main(){
    std::ifstream fin("ami33.block", std::ifstream::in);
    std::ofstream fout1("initFloorplan.txt", std::ofstream::out);
    std::ofstream fout2("initTree.txt", std::ofstream::out);

    BStarTree BST;
    // declare vector of blocks in main
    std::vector<Block> blockList;

    // do io
    std::string s, ss;
    int x, y, bbw, bbh, blockNum, terminalNum, netNum;
    fin >> s >> bbw >> bbh >> s >> blockNum >> s >> terminalNum;
    for (int i = 0; i < blockNum; i++){
        fin >> s >> x >> y;
        blockList.push_back(Block(i, HARD_BLOCK, s, x*y, x, y));
    }


    // init bstar tree here (might be illegal floorplan)
    BST.init(blockList);

    // do perturb here
    BST.perturbRotateBlock(&(blockList[0]));

    // print btree and floorplan
    BST.printFloorplan(fout1, bbw, bbh);
    BST.printTree(fout2);
    

    return 0;
}