#include <iostream>
#include <iomanip>
#include <assert.h>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include "Block.h"
#include "BStarTree.h"

using namespace std;



class Connection{
public:
    Block *first;
    Block *second;
    int num_nets;
};

int divisorCloseRoot(long long num);
double FastSAProb(double deltaC, double temp);
double CalculateConnection(vector <Connection*>, int);
void printBlock(Block *block);

int main(int argc, char *argv[]){

    //PARAM declerations
    //-------- SA Related ---------------

    double DELTA_AVG;
    double DELTA_COST;
    double A_NORM;
    double W_NORM;

    //-------- Input Related ---------------
    //CHIP related variables
    int CHIP_WIDTH, CHIP_HEIGHT;

    // SOFTMODULE related variables
    int NUMBER_OF_SOFT_MODULES;
    vector <Block*> soft_modules_vector;

    // FIXEDMODULE related variables
    int NUMBER_OF_FIXED_MODULES;
    vector <Block*> fixed_modules_vector;

    // CONNECTIONS related variables
    int NUMBER_OF_CONNECTIONS;
    vector <Connection*> connections_vector;


    int NUMBER_OF_BLOCKS;
    vector <Block*> allblock_vector;
    //--------------------------------------


    /*Input Section: read blocks from args*/
    assert(argc == 3);
    fstream fin, fout;
    fin.open(argv[1], ios::in);
    fout.open(argv[2], ios::out);
    assert(fin.is_open());
    
    //tmp variable
    string readstr;
    int readint;
    
    fin >> readstr >> CHIP_WIDTH >> CHIP_HEIGHT;
    // cout << "readstr: " << readstr << endl;
    // cout << "cw: " << CHIP_WIDTH << endl;
    // cout << "ch: " << CHIP_HEIGHT << endl;

    fin >> readstr >> NUMBER_OF_SOFT_MODULES;
    for(int soft_idx = 0; soft_idx < NUMBER_OF_SOFT_MODULES; soft_idx++){
        Block *soft_block = new Block;
        fin >> readstr >> readint;
        soft_block->blocktype = SOFT_BLOCK;
        soft_block->blockname = readstr;
        soft_block->id = soft_idx;
        soft_block->area = readint;
        soft_block->block_x = 0; // initialize as 0?
        soft_block->block_y = 0; // initialize as 0?
        soft_block->width = divisorCloseRoot(readint);
        soft_block->height = readint/(soft_block->width);
        soft_modules_vector.push_back(soft_block);
    }


    // for(int i = 0; i < NUMBER_OF_SOFT_MODULES; i++){
    //     cout << soft_modules_vector[i]->blockname;
    //     printf("%10d, %10d x %10d\n",soft_modules_vector[i]->area, soft_modules_vector[i]->width, soft_modules_vector[i]->height);
    // }


    fin >> readstr >> NUMBER_OF_FIXED_MODULES;
    for(int fixed_idx = 0; fixed_idx < NUMBER_OF_FIXED_MODULES; fixed_idx++){
        Block *fixed_block = new Block;
        fixed_block->blocktype = HARD_BLOCK;
        fin >> fixed_block->blockname;
        fixed_block->id = 100 + fixed_idx;
        fin >> fixed_block->block_x >> fixed_block->block_y;
        fin >> fixed_block->width >> fixed_block->height;
        fixed_block->area = (fixed_block->width) * (fixed_block->height);
        fixed_modules_vector.push_back(fixed_block);
    }
    // for(int i = 0; i < NUMBER_OF_FIXED_MODULES; i++){
    //     Block *target = fixed_modules_vector[i];
    //     cout << "[" << target->blockname <<", "<<target->id<<"]" << target->block_x <<" " <<  target->block_y << " ";
    //     cout << target->width << " " <<target->height <<endl;
    // }
    

    fin >> readstr >> NUMBER_OF_CONNECTIONS;
    // cout << "connect: " << NUMBER_OF_CONNECTIONS<<endl;
    int found_node;
    for(int pair_idx = 0; pair_idx < NUMBER_OF_CONNECTIONS; pair_idx++){
        Connection *conn = new Connection;
        
        //find the first ndoe;
        fin >> readstr;
        found_node = 0;
        for(int i = 0; i < NUMBER_OF_SOFT_MODULES; i++){
            if(soft_modules_vector[i]->blockname == readstr){
                conn->first = soft_modules_vector[i];
                found_node = 1;
                break;
            }
        }
        if(!found_node){
            for(int i = 0; i < NUMBER_OF_FIXED_MODULES; i++){
                if(fixed_modules_vector[i]->blockname == readstr){
                    conn->first = fixed_modules_vector[i];
                    found_node = 1;
                    break;
                }
            }
        }
        assert(found_node ==1);
        //find the second node
        fin >> readstr;
        found_node = 0;
        for(int i = 0; i < NUMBER_OF_SOFT_MODULES; i++){
            if(soft_modules_vector[i]->blockname == readstr){
                conn->second = soft_modules_vector[i];
                found_node = 1;
                break;
            }
        }
        if(!found_node){
            for(int i = 0; i < NUMBER_OF_FIXED_MODULES; i++){
                if(fixed_modules_vector[i]->blockname == readstr){
                    conn->second = fixed_modules_vector[i];
                    found_node = 1;
                    break;
                }
            }
        }
        assert(found_node ==1);

        fin >> conn->num_nets;
        connections_vector.push_back(conn);

    }

    // for(int i = 0; i < NUMBER_OF_CONNECTIONS; i++){
    //     cout << "[" << connections_vector[i]->first->blockname << "]" << "--- ";
    //     cout << connections_vector[i]->num_nets << " --->";
    //     cout << "[" << connections_vector[i]->second->blockname << "]" <<endl;
    // }

    /*Start pushing blocks into B*-Tree* and initialize B*-Tree */
    BStarTree BST;
    int floorplan_width, floorplan_height;
    std::ofstream fout1("initFloorplan.txt", std::ofstream::out);
    std::ofstream fout2("initTree.txt", std::ofstream::out);

    NUMBER_OF_BLOCKS = NUMBER_OF_SOFT_MODULES + NUMBER_OF_FIXED_MODULES;
    for(int soft_idx = 0; soft_idx < NUMBER_OF_SOFT_MODULES; soft_idx++){
        allblock_vector.push_back(soft_modules_vector[soft_idx]);
    }
    for(int hard_idx = 0; hard_idx < NUMBER_OF_FIXED_MODULES; hard_idx++){
        allblock_vector.push_back(fixed_modules_vector[hard_idx]);
    }
    printf("Packing..\n");
    for(int i = 0; i < (NUMBER_OF_SOFT_MODULES+NUMBER_OF_FIXED_MODULES); i++){
        printBlock(allblock_vector[i]);
    }
    printf("Done packing..\n");

    BST.init(allblock_vector);
    BST.render();
    
    BST.boundingBox(&floorplan_width, &floorplan_height);
    
    printf("W: %d, H:% d\n", floorplan_width, floorplan_height);
    printf("Conn: %lf\n", CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS));
    printf("Initialized..\n");
    for(int i = 0; i < (NUMBER_OF_SOFT_MODULES+NUMBER_OF_FIXED_MODULES); i++){
        printBlock(allblock_vector[i]);
    }


    // printf("After Render...\n");
    // for(int i = 0; i < NUMBER_OF_FIXED_MODULES; i++){
    //     printBlock(&blockList[i]);
    // }
    // for(int i = NUMBER_OF_FIXED_MODULES; i < (NUMBER_OF_FIXED_MODULES+NUMBER_OF_SOFT_MODULES); i++){
    //     printBlock(&blockList[i]);
    // }


    // // printf("Before rotate: (%d, %d @(%d, %d))", blockList[3].width, blockList[3].height, blockList[3].block_x, blockList[3].block_y);
    // // printBlock(&blockList[3]);
    // int result = BST.perturbRotateBlock(&(blockList[3]));
    // printf("Rotate reuslt: %d\n",result);
    // BST.render();
    // printf("W: %d, H:% d\n", floorplan_width, floorplan_height);
    // printf("Conn: %lf\n", CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS));


    // result = BST.perturbMoveBlock(&(blockList[3]), &(blockList[1]));
    // printf("Rotate reuslt: %d\n",result);
    // BST.render();
    // printf("W: %d, H:% d\n", floorplan_width, floorplan_height);
    // printf("Conn: %lf\n", CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS));

    // //final print
    // BST.printFloorplan(fout1, CHIP_WIDTH, CHIP_HEIGHT);
    // BST.printTree(fout2);

}

int divisorCloseRoot(long long num){

    if (num < 4) return 1;
    else{
        int divisor = 1;
        for (long long i = 2; i < num; i++) {
            if (i * i == num) return i;
            if (i * i < num && num % i == 0) divisor = i;
            if (i * i > num) return divisor;
        }
    }

    return -1;
}

double FastSAProb(double deltaC, double temp){
    double power = -deltaC/temp;
    double answer = exp(power);
    return (answer < 1)? answer : 1;
}

double CalculateConnection(vector <Connection*> connections_vector, int NUMBER_OF_CONNECTIONS){
    double answer = 0;
    double distance;
    double b1c_x, b1c_y, b2c_x, b2c_y;
    
    for(int conn_idx = 0; conn_idx < NUMBER_OF_CONNECTIONS; conn_idx++){
        b1c_x = connections_vector[conn_idx]->first->block_x + (connections_vector[conn_idx]->first->width / 2);
        b1c_y = connections_vector[conn_idx]->first->block_y + (connections_vector[conn_idx]->first->height / 2);

        b2c_x = connections_vector[conn_idx]->second->block_x + (connections_vector[conn_idx]->second->width / 2);
        b2c_y = connections_vector[conn_idx]->second->block_y + (connections_vector[conn_idx]->second->height/ 2);
        
        distance = 0;
        if(b1c_x > b2c_x) distance += (b1c_x - b2c_x);
        else distance += (b2c_x - b1c_x);

        if(b1c_y > b2c_y) distance += (b1c_y - b2c_y);
        else distance += (b2c_y - b1c_y);
        
        answer += connections_vector[conn_idx]->num_nets * distance;

    }
    return answer;
}

void printBlock(Block *block){
    if(block->blocktype == HARD_BLOCK){
        printf("H[");
    }else{ // SOFT_BLOCK
        printf("S[");
    }

    cout << setw(8)<<block->blockname << "(" <<block->id <<")]";
    printf(", A: %10d", block->area);
    printf(", W: %7d, H: %7d, ",block->width, block->height);
    printf(", @X: %7d, @Y: %7d\n",block->block_x, block->block_y);

}