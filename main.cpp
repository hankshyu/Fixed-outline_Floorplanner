#include <iostream>
#include <iomanip>
#include <assert.h>
#include <math.h>
#include <fstream>
#include <stdlib.h>
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
int rollPetrubDice(double , double , double , double);
int rollSoftBlocks(int);


int main(int argc, char *argv[]){

    //PARAM declerations
    srand(17);
    //-------- SA Related ---------------

    double PETURB_RATIO_ROTATE  = 0.00;
    double PETURB_RATIO_RESIZE  = 0.00;
    double PETURB_RATIO_MOVE    = 0.00;
    double PETURB_RATIO_SWAP    = 1.00;

    //Cost function is defined as: Cost_alpha * Area + Cost_beta * WireLength + (1-Cost_alpha-Cost_beta) * (R-RStar)

    //1. Conduct initial peturbations for (block num) * INITIAL_PETURB_RATIO to collect data;
    double INITIAL_PETURB_RATIO = 15.0;
    //These is collected at initial perturbation
    double DELTA_AVG;   //This is collected at initial perturbation
    double A_NORM;
    double W_NORM;
    double R_NORM;

    double DELTA_COST;  //This is collected at each rounds(few peturbations)

    //newAspectRatio = currentAspectRatio * PETURB_RESIZE_STEP;
    double PETURB_RESIZE_STEP = 1.5;

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
    BST.init(allblock_vector);
    printf("Packing..\n");
    for(int i = 0; i < (NUMBER_OF_SOFT_MODULES+NUMBER_OF_FIXED_MODULES); i++){
        printBlock(allblock_vector[i]);
    }
    printf("Done packing..\n");

    // /*Prepare for SA section: Peturb BST for */
    int initial_peturbtype_dice_roll;
    int target0, target1;
    // for(int init_peturb_idx = 0; init_peturb_idx < (NUMBER_OF_BLOCKS * INITIAL_PETURB_RATIO); init_peturb_idx++){
    for(int init_peturb_idx = 0; init_peturb_idx < 500; init_peturb_idx++){

        initial_peturbtype_dice_roll = rollPetrubDice(PETURB_RATIO_ROTATE, PETURB_RATIO_RESIZE, PETURB_RATIO_MOVE, PETURB_RATIO_SWAP);
        
        int target0 = rollSoftBlocks(NUMBER_OF_SOFT_MODULES);
        int target1 = rollSoftBlocks(NUMBER_OF_SOFT_MODULES);
        while(target1 == target0) target1 = rollSoftBlocks(NUMBER_OF_SOFT_MODULES);
        double original_ratio;

       printf("InitP[%3d] OP:",init_peturb_idx);
        
        switch(initial_peturbtype_dice_roll){
            case 0:     // rotate
                BST.perturbRotateBlock(soft_modules_vector[target0]);
                cout <<"Rotate SB" << target0;
                break;
            case 1:     // resize
                original_ratio = (soft_modules_vector[target0]->width) / (soft_modules_vector[target0]->height);
                BST.perturbResizeSoftBlock(soft_modules_vector[target0], original_ratio * PETURB_RESIZE_STEP);
                cout <<"Resize SB"<<target0 << "(" <<original_ratio << " -> " << original_ratio * PETURB_RESIZE_STEP <<")";
                break;
            case 2:     // move
                BST.perturbMoveBlock(soft_modules_vector[target0],soft_modules_vector[target1]);
                cout <<"Move SB" <<target0 << "to parent SB"<<target1;
                break;
            default:    //swap
                BST.perturbSwapNode(soft_modules_vector[target0], soft_modules_vector[target1]);
                cout <<"Swap SB" <<target0 << "with SB"<<target1;
        }
        
        cout << endl;
        BST.render();
        BST.boundingBox(&floorplan_width, &floorplan_height);
        
        cout <<"L:"<<BST.getisLegal()<<", "<<floorplan_width <<", "<<floorplan_height;
        cout <<  CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS)<<endl; 
    }

    
    // BST.boundingBox(&floorplan_width, &floorplan_height);
    
    // printf("W: %d, H:% d\n", floorplan_width, floorplan_height);
    // printf("Conn: %lf\n", CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS));
    // printf("Initialized..\n");
    // for(int i = 0; i < (NUMBER_OF_SOFT_MODULES+NUMBER_OF_FIXED_MODULES); i++){
    //     printBlock(allblock_vector[i]);
    // }



    // int result = BST.perturbRotateBlock(soft_modules_vector[2]);
    // BST.render();
    // printf("Rotate reuslt: %d\n",result);
    // BST.boundingBox(&floorplan_width, &floorplan_height);
    // printf("W: %d, H:% d\n", floorplan_width, floorplan_height);
    // printf("Conn: %lf\n", CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS));
    //     for(int i = 0; i < (NUMBER_OF_SOFT_MODULES+NUMBER_OF_FIXED_MODULES); i++){
    //     printBlock(allblock_vector[i]);
    // }




    // result = BST.perturbMoveBlock(soft_modules_vector[1], fixed_modules_vector[2]);
    // BST.render();
    // printf("Move reuslt: %d\n",result);
    // BST.boundingBox(&floorplan_width, &floorplan_height);
    // printf("W: %d, H:% d\n", floorplan_width, floorplan_height);
    // printf("Conn: %lf\n", CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS));
    //     for(int i = 0; i < (NUMBER_OF_SOFT_MODULES+NUMBER_OF_FIXED_MODULES); i++){
    //     printBlock(allblock_vector[i]);
    // }

    // result = BST.perturbSwapNode(soft_modules_vector[2], soft_modules_vector[1]);
    // BST.render();
    // printf("Swap reuslt: %d\n",result);
    // BST.boundingBox(&floorplan_width, &floorplan_height);
    // printf("W: %d, H:% d\n", floorplan_width, floorplan_height);
    // printf("Conn: %lf\n", CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS));
    //     for(int i = 0; i < (NUMBER_OF_SOFT_MODULES+NUMBER_OF_FIXED_MODULES); i++){
    //     printBlock(allblock_vector[i]);
    // }

    // result = BST.perturbResizeSoftBlock(soft_modules_vector[2], 0.5);
    // BST.render();
    // printf("resize reuslt: %d\n",result);
    // BST.boundingBox(&floorplan_width, &floorplan_height);
    // printf("W: %d, H:% d\n", floorplan_width, floorplan_height);
    // printf("Conn: %lf\n", CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS));
    //     for(int i = 0; i < (NUMBER_OF_SOFT_MODULES+NUMBER_OF_FIXED_MODULES); i++){
    //     printBlock(allblock_vector[i]);
    // }

    //final print
    BST.printFloorplan(fout1, CHIP_WIDTH, CHIP_HEIGHT);
    BST.printTree(fout2);

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
    printf(", W: %7d, H: %7d",block->width, block->height);
    printf(", @X: %7d, @Y: %7d\n",block->block_x, block->block_y);

}

int rollPetrubDice(double rotate, double resize, double move, double swap){
    //create random between 0 and 9999_9999
    const int granularity = 100000000;
    int randomvalue = rand() % granularity;
    int rotate_b = granularity * rotate;
    int resize_b = granularity * (rotate + resize);
    int move_b = granularity * (rotate + resize + move);
    if(randomvalue < rotate_b) return 0;
    else if (randomvalue < resize_b) return 1;
    else if (randomvalue < move_b) return 2;
    else return 3;

    return -1; // this would never hit
}

inline int rollSoftBlocks(int soft_block_num){
    return rand() % soft_block_num;
}