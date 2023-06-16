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
double absdiffdouble(double , double);


int main(int argc, char *argv[]){

    //PARAM declerations
    srand(17);
    //-------- SA Related ---------------

    double PETURB_RATIO_ROTATE  = 0.25;
    double PETURB_RATIO_RESIZE  = 0.25;
    double PETURB_RATIO_MOVE    = 0.25;
    double PETURB_RATIO_SWAP    = 0.25;

    //Cost function is defined as: COST_ALPHA * Area + COST_BETA * WireLength + (1 - COST_ALPHA - COST_BETA) * abs(Aspect Ratio - R_Star);
    double COST_ALPHA   = 0.35;
    double COST_BETA    = 0.5;
    double R_Star;      //Average Aspect Ratio

    //1. Conduct initial peturbations for (block num) * INITIAL_PETURB_RATIO to collect data;
    double INITIAL_PETURB_RATIO = 50.0;
    //These are collected at initial perturbation
    double Delta_avg;
    double A_norm;
    double W_norm;
    double R_norm;  // Aspect Ratio is defined as: height/width (H/W)

    //2. Start SA
    double SA_INITIAL_ACCEPT_RATE_P = 0.97;
    double SA_temperature;
    int SA_PETURB_PET_TEMPERATURE = 10;
    
    int SA_RUN_PHASE2_RPUNDS = 30;    //Runs this much phase 2
    int SA_RUN_PHASE3_RPUNDS = 170;   //Runs this much phase 3


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
    R_Star = CHIP_HEIGHT/CHIP_WIDTH;
    for(int soft_idx = 0; soft_idx < NUMBER_OF_SOFT_MODULES; soft_idx++){
        allblock_vector.push_back(soft_modules_vector[soft_idx]);
    }
    for(int hard_idx = 0; hard_idx < NUMBER_OF_FIXED_MODULES; hard_idx++){
        allblock_vector.push_back(fixed_modules_vector[hard_idx]);
    }
    printf("Output Input data..\n");
    for(int i = 0; i < (NUMBER_OF_SOFT_MODULES+NUMBER_OF_FIXED_MODULES); i++){
        printBlock(allblock_vector[i]);
    }
    BST.init(allblock_vector);
    BST.boundingBox(&floorplan_width, &floorplan_height);
    // BST.saveCurrent();
    
    printf("Init called for BST..\n");
    for(int i = 0; i < (NUMBER_OF_SOFT_MODULES+NUMBER_OF_FIXED_MODULES); i++){
        printBlock(allblock_vector[i]);
    }


    cout <<"L:"<<BST.getisLegal()<<",W: "<<floorplan_width <<",H: "<<floorplan_height << ", HPWL: ";
    cout <<  CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS)<<endl; 



    /*Prepare for SA section: Peturb BST for few times to collect the average values of perturbation */
    assert(PETURB_RATIO_ROTATE > 0 && PETURB_RATIO_ROTATE < 1);
    assert(PETURB_RATIO_RESIZE > 0 && PETURB_RATIO_RESIZE < 1);
    assert(PETURB_RATIO_MOVE > 0 && PETURB_RATIO_MOVE < 1);
    assert(PETURB_RATIO_SWAP > 0 && PETURB_RATIO_SWAP < 1);
    assert((PETURB_RATIO_ROTATE + PETURB_RATIO_RESIZE + PETURB_RATIO_MOVE + PETURB_RATIO_SWAP) == 1 );
    assert(COST_ALPHA > 0 && COST_ALPHA < 1);
    assert(COST_BETA > 0 && COST_BETA < 1);
    assert((COST_ALPHA + COST_BETA)<= 1);
    
    int init_peturbtype_dice_roll;
    int target0, target1;   //Roll the softblocks to peturb;
    bool init_peturb_tosquare;
    Delta_avg = A_norm = W_norm = R_norm = 0;

    double init_peturb_cost, init_peturb_last_cost;
    int init_peturb_uphill_count = 0;
    double init_peturb_area, init_petrub_wirelength, init_peturb_ratio;

    for(int init_peturb_idx = 0; init_peturb_idx < (NUMBER_OF_BLOCKS * INITIAL_PETURB_RATIO); init_peturb_idx++){

        init_peturbtype_dice_roll = rollPetrubDice(PETURB_RATIO_ROTATE, PETURB_RATIO_RESIZE, PETURB_RATIO_MOVE, PETURB_RATIO_SWAP);
        
        int target0 = rollSoftBlocks(NUMBER_OF_SOFT_MODULES);
        int target1 = rollSoftBlocks(NUMBER_OF_SOFT_MODULES);
        while(target1 == target0) target1 = rollSoftBlocks(NUMBER_OF_SOFT_MODULES);
        double original_ratio;

        printf("InitP[%3d] OP:",init_peturb_idx);
        
        switch(init_peturbtype_dice_roll){
            case 0:     // rotate
                BST.perturbRotateBlock(soft_modules_vector[target0]);
                cout <<"Rotate SB" << target0;
                break;
            case 1:     // resize
                init_peturb_tosquare = (rand()%2)? true : false;
                BST.perturbResizeSoftBlock(soft_modules_vector[target0], init_peturb_tosquare);
                cout <<"Resize SB"<<target0 << "(TS: " << init_peturb_tosquare<< ")";
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
        //Calculate the outcome of this peturbation
        init_peturb_area = floorplan_width * floorplan_height;
        init_petrub_wirelength = CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS);
        
        init_peturb_ratio = floorplan_height/ floorplan_width;
        init_peturb_ratio = absdiffdouble(R_Star, init_peturb_ratio);

        init_peturb_last_cost = init_peturb_cost;   //latch the old result for calculating Delta
        init_peturb_cost = (COST_ALPHA * init_peturb_area) + (COST_BETA*init_petrub_wirelength) + 
        ((1 - COST_ALPHA - COST_BETA) * init_peturb_ratio);
        if(init_peturb_idx == 0) init_peturb_last_cost = init_peturb_cost;

        //collecting the peturbation data to calculate average
        if(init_peturb_idx == 0){
            A_norm = init_peturb_area;
            W_norm = init_petrub_wirelength;
            R_norm = init_peturb_ratio;
        }else{
            A_norm = ((double)init_peturb_idx/(double)(init_peturb_idx+1))*A_norm + (1/(double)(init_peturb_idx+1))*init_peturb_area;
            W_norm = ((double)init_peturb_idx/(double)(init_peturb_idx+1))*W_norm + (1/(double)(init_peturb_idx+1))*init_petrub_wirelength;
            R_norm = ((double)init_peturb_idx/(double)(init_peturb_idx+1))*R_norm + (1/(double)(init_peturb_idx+1))*init_peturb_ratio;
        }
        if((init_peturb_idx != 0) && (init_peturb_last_cost < init_peturb_cost)){ // an uphill movement
            if(init_peturb_uphill_count == 0){
                Delta_avg = (init_peturb_cost - init_peturb_last_cost);
            }else{
                Delta_avg = ((double)init_peturb_uphill_count/(double)(init_peturb_uphill_count+1))*Delta_avg 
                        + (1/(double)(init_peturb_uphill_count+1))*(init_peturb_cost - init_peturb_last_cost);
            }
            init_peturb_uphill_count++;
        } 


        cout <<"["<< init_peturb_idx << "]";
        cout <<"L:"<<BST.getisLegal()<<",W: "<<floorplan_width <<",H: "<<floorplan_height << ",A: "<< floorplan_width*floorplan_height << ", HPWL: ";
        cout <<  CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS)<<endl; 
    }
    // A_norm /= (NUMBER_OF_BLOCKS * INITIAL_PETURB_RATIO);
    // W_norm /= (NUMBER_OF_BLOCKS * INITIAL_PETURB_RATIO);
    // R_norm /= (NUMBER_OF_BLOCKS * INITIAL_PETURB_RATIO);
    // Delta_avg /= init_peturb_uphill_count;

    cout << "A_norm: " << A_norm <<endl;
    cout << "W_norm: " << W_norm <<endl;
    cout << "R_norm: " << R_norm <<endl;
    cout << "Delta_avg: " << Delta_avg <<endl;

    /*Start SA process, this is when n = 1*/
    // BST.loadPrevCurrent(); // rollback to init
    // SA_temperature = Delta_avg  / log(SA_INITIAL_ACCEPT_RATE_P);
    // cout << "Initial SA T: "<<SA_temperature;

    // int sa_peturb_dice_roll;
    // int sa_target0, sa_target1;

    // double sa_peturb_area, sa_petrub_wirelength, sa_peturb_ratio;
    // for(int sa_n = 0; sa_n  < (1 + SA_RUN_PHASE2_RPUNDS + SA_RUN_PHASE3_RPUNDS); sa_n++){
    // for(int sa_n = 0; sa_n  < (1 ); sa_n++){

    //     for(int sa_pt = 0; sa_pt < SA_PETURB_PET_TEMPERATURE; sa_pt++){
    //         sa_peturb_dice_roll = rollPetrubDice(PETURB_RATIO_ROTATE, PETURB_RATIO_RESIZE, PETURB_RATIO_MOVE, PETURB_RATIO_SWAP);
            
    //         sa_target0 = rollSoftBlocks(NUMBER_OF_SOFT_MODULES);
    //         sa_target1 = rollSoftBlocks(NUMBER_OF_SOFT_MODULES);
    //         while(target1 == target0) target1 = rollSoftBlocks(NUMBER_OF_SOFT_MODULES);

    //         // printf("P1[%3d] OP:",init_peturb_idx);
    //         switch(sa_peturb_dice_roll){
    //             case 0:     // rotate
    //                 BST.perturbRotateBlock(soft_modules_vector[sa_target0]);
    //                 // cout <<"Rotate SB" << sa_target0;
    //                 break;
    //             case 1:     // resize
    //                 init_peturb_tosquare = (rand()%2)? true : false;
    //                 BST.perturbResizeSoftBlock(soft_modules_vector[sa_target0], init_peturb_tosquare);
    //                 // cout <<"Resize SB"<<sa_target0 << "(TS: " << init_peturb_tosquare<< ")";
    //                 break;
    //             case 2:     // move
    //                 BST.perturbMoveBlock(soft_modules_vector[sa_target0],soft_modules_vector[sa_target1]);
    //                 // cout <<"Move SB" <<sa_target0 << "to parent SB"<<target1;
    //                 break;
    //             default:    //swap
    //                 BST.perturbSwapNode(soft_modules_vector[sa_target0], soft_modules_vector[sa_target1]);
    //                 // cout <<"Swap SB" <<sa_target0 << "with SB"<<target1;
    //         }
    //         // cout << endl;
    //         BST.render();
    //     }
    //     BST.boundingBox(&floorplan_width, &floorplan_height);
    //     sa_peturb_area = floorplan_width * floorplan_height;
    //     sa_petrub_wirelength = CalculateConnection(connections_vector, NUMBER_OF_CONNECTIONS);
        
    //     sa_peturb_ratio = floorplan_height/ floorplan_width;
    //     sa_peturb_ratio = absdiffdouble(R_Star, init_peturb_ratio);
        
    //     cout << "Stage 1:"<<endl;
    //     cout <<"L:"<<BST.getisLegal()<<",W: "<<floorplan_width <<",H: "<<floorplan_height << ",A: "<< floorplan_width*floorplan_height << ", HPWL: ";
    //     cout <<  sa_petrub_wirelength<<endl; 


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

inline double absdiffdouble(double d1, double d2){
    if(d1 > d2) return d1 - d2;
    else return d2 - d1;

    return -1;
}