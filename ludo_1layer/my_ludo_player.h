#ifndef MY_LUDO_PLAYER_H
#define MY_LUDO_PLAYER_H
#include <iostream>
#include "positions_and_dice.h"
#include <random>

#include "neuralnetwork.cpp"
#include "gamerules.cpp"

using namespace std;



struct LearningData
{
    bool terminal = false;
    
    vector<int> rel_pos;
    int dice;
    int action;
    
    vector<int> valid_moves;
};

class my_ludo_player
{
private:
    vector<int> pos_start_of_turn;
    vector<int> pos_end_of_turn;
    int dice_roll;
    int make_decision();
    
    GameRules theLawsOfTheUniverse;
    
    random_device gen;
    uniform_int_distribution<int> dist;
    
public:
    my_ludo_player();
    int start_turn(positions_and_dice relative);
    bool post_game_analysis(std::vector<int> relative_pos);
    
    void RecieveEndState(vector<int> rel_pos);
    
    //vector<vector<int>> states;
    
    vector<double> RelPos2State(vector<int> rel_pos);
    vector<double> GlobPos2State(vector<int> glob_pos);
    Matrix ToNetworkInput(vector<int> a_rel_pos, int a_dice_roll, int an_action);
    
    ANN* network;
    bool learning = false;
    
    int percent = 0; //for epsilon-greedy
    
    vector<LearningData> data;
};

#endif // MY_LUDO_PLAYER_H
