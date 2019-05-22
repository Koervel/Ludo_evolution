#ifndef LUDO_PLAYER_H
#define LUDO_PLAYER_H
#include <iostream>
#include "positions_and_dice.h"

class ludo_player{
private:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    int dice_roll;
    int make_decision();
public:
    ludo_player();
    int start_turn(positions_and_dice relative);
    bool post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_H
