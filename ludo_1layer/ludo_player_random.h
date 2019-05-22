#ifndef LUDO_PLAYER_RANDOM_H
#define LUDO_PLAYER_RANDOM_H
#include <iostream>
#include "positions_and_dice.h"
#include <random>

class ludo_player_random{
private:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    int dice_roll;
    std::random_device rd;
    std::mt19937 gen;

    int make_decision();
public:
    ludo_player_random();
    int start_turn(positions_and_dice relative);
    bool post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_RANDOM_H

