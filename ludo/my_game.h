#ifndef MY_GAME_H
#define MY_GAME_H

#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include "positions_and_dice.h"

using namespace std;

class my_game
{
public:
    //================
    //   Attributes
    //================
    random_device rd;
    mt19937 gen;
    
    bool game_complete;
    bool turn_complete;
    positions_and_dice relative;
    int dice_result;
    int color;
    std::vector<int> player_positions;
    
    //link to the players
    int (*start_turn)(positions_and_dice, int);
    bool (*post_game_analysis)(vector<int>, int);
    vector<int> order;
    
    //int trainingPlayer = -1;
    void (*SendEndState)(int, vector<int>) = nullptr;
    
    //for info/data
    bool info = false;
    
    bool log_wins = false;
    bool log_utils = false;
    
    string logfile_path;
    string game_number;
    
    vector<vector<int>> utils_buffer;
    
    //===============
    //   Functions
    //===============
    my_game();
    std::vector<int> relativePosition();
    int isStar(int index);
    bool isGlobe(int index);
    int isOccupied(int index); //see if it is occupied and return the piece number
    int rel_to_fixed(int relative_piece_index);
    void send_them_home(int index);
    void move_start(int fixed_piece);
    int next_turn();
    void rollDice()
    {
        std::uniform_int_distribution<> dis(1, 6);
        dice_result = dis(gen);
    }
    int getDiceRoll() {return dice_result; }
    void reset();
    void turnComplete(bool win);
    bool movePiece(int relative_piece); //check game rules
    int run();
    
    vector<int> CalcUtilities();
};

#endif // MY_GAME_H
