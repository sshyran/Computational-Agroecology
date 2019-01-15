#include <iostream>
#include "plant.h"

using namespace std;

Plant::Plant(int id)
{
    this.id = id;


    // Populate states data structure based on database
    liveState = 0;
    dieState = 0;
    living = true;
    flowering = false;
    dead = false;
}

//flesh this function out as necessary
//assume water and nutrient data are stored in env 1 and 2 respectively
bool Plant::checkNeeds(int env[]){
    if(env[1] >= waterNeeds && env[2] >= nutrientNeeds){
        return true;
    }
    else{
        return false;
    }
}
void Plant::transition(int env[])
{
    // if env fulfills needs then increment liveState
    // if liveState reaches threshold then advance curState
    // if env does not fulfill needs of current state increment deadState
    // kill plant if dieState exceeds threshold
    if(living == 1){
        if(checkNeeds(env[])){
            liveState++;
        }
        else{
            dieState++;
        }
    }
}







