#include "my_game.h"

my_game::my_game():
    game_complete(false),
    turn_complete(true),
    relative(),
    dice_result(1),
    rd(),
    gen(rd()),
    color(3),
    player_positions({-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1})
{
}


void my_game::reset()
{
    game_complete = false;
    turn_complete = true;
    player_positions.clear();
    for (int i = 0; i < 16; ++i)
    {
         player_positions.push_back(-1);
    }
    color = 3;
    
    utils_buffer.clear();
}

int my_game::rel_to_fixed(int relative_piece_index)
{
    return relative_piece_index + color * 4;
}

int my_game::isStar(int index)
{
    if (index == 5  || index == 18 || index == 31 || index == 44)
        return 6;
    else if (index == 11 || index == 24 || index == 37 || index == 50)
        return 7;
    return 0;
}

int my_game::isOccupied(int index)
{
    //returns number of people
    int number_of_people = 0;
    if (index != 99)
    {
        for (int i = 0; i < player_positions.size(); ++i)
        {
            if (i < color*4 || i >= color*4 + 4)
                if (player_positions[i] == index)
                    ++number_of_people;
        }
    }
    return number_of_people;
}

bool my_game::isGlobe(int index)
{
    if (index < 52)
        if (index % 13 == 0 || (index - 8) % 13 == 0 || isOccupied(index) > 1)
            return true;
    return false;
}

void my_game::send_them_home(int index)
{
    for (size_t i = 0; i < player_positions.size(); ++i)
    {
        if (i < color*4 || i >= color*4 + 4)
        {
            //different color
            if(player_positions[i] == index)
                player_positions[i] = -1;
        }
    }
}

void my_game::move_start(int fixed_piece)
{
    if (dice_result == 6 && player_positions[fixed_piece] < 0)
    {
        player_positions[fixed_piece] = color*13; //move me to start
        send_them_home(color*13); //send pieces home if they are on our start
    }
}

int my_game::next_turn()
{
    if (game_complete)
    {
        return 0;
    }
    switch (color)
    {
        case 0:
        case 1:
        case 2:
            ++color;
            break;
        case 3:
        default:
            color = 0;
            break;
    }
    rollDice();
    relative.dice = getDiceRoll();
    relative.pos = relativePosition();
    
    int decision = start_turn(relative, order[color]);
    bool res = movePiece(decision);
    turnComplete(res);
    
//    int decision;
//    bool res;
//    switch (color)
//    {
//        case 0:
//            decision = start_turn(relative, order[0]);
//            res = movePiece(decision);
//            turnComplete(res);
//            break;
//        case 1:
//            decision = start_turn(relative, order[1]);
//            res = movePiece(decision);
//            turnComplete(res);
//            break;
//        case 2:
//            decision = start_turn(relative, order[2]);
//            res = movePiece(decision);
//            turnComplete(res);
//            break;
//        case 3:
//            decision = start_turn(relative, order[3]);
//            res = movePiece(decision);
//            turnComplete(res);
//        default:
//            break;
//    }

    return 0;
}

bool my_game::movePiece(int relative_piece)
{
    int fixed_piece = rel_to_fixed(relative_piece);
    int modifier = color * 13;
    int relative_pos = player_positions[fixed_piece];
    int target_pos = 0;
    if (player_positions[fixed_piece] == -1)
    {
        move_start(fixed_piece);
    }
    else
    {
        //convert to relative position
        if (relative_pos == 99)
        {
            std::cout << "I thought this would be it!" << endl;
        }
        else if (relative_pos < modifier)
        {
            relative_pos = relative_pos + 52 - modifier;
        }
        else if (relative_pos == 51)
        {
            //if people land on 51, they shouldn't be sent to goal stretch
            switch (color)
            {
            case 0:
                relative_pos = 51;
                break;
            case 1:
                relative_pos = 38;
                break;
            case 2:
                relative_pos = 25;
                break;
            case 3:
                relative_pos = 12;
                break;
            }
        }
        else if ( relative_pos > /*50*/ 51) //apply fix to relative positions here also!!!
        {
            //relative_pos = relative_pos - color * 5 - 1;
            
            //my fix:
            int tmp = relative_pos - 52;
            if (tmp < color*5)
            {
                tmp = (tmp + 20 - color*5);
            }
            else //if (tmp >= color*5)
            {
                tmp = (tmp - color*5);
            }
            relative_pos = tmp + 51;
        }
        else
        {
            //if(relative >= modifier)
            relative_pos = relative_pos - modifier;
        }

        //add dice roll
        relative_pos += dice_result; //this works because we are in relative coordinates

        int jump = isStar(relative_pos); //return 0 | 6 | 7
        if (jump)
        {
            if (jump + relative_pos == 57)
            {
                relative_pos = 56; //the last star move you to the goal?!
            }
            else
            {
                relative_pos += jump;
            }
        }
        
        //special case checks (and convert back to absolute coordinates)
        if (relative_pos > 56 && relative_pos < 72)
        {
            // go back
            target_pos = 56-(relative_pos-56) + color * 5 + 1; //trust me
        }
        else if (relative_pos == 56 || relative_pos >= 99)
        {
            target_pos = 99;
        }
        else if (relative_pos > 50)
        {
            // goal stretch
            target_pos = relative_pos + color * 5 + 1;
        }
        else
        {
            int new_pos = relative_pos + modifier;
            if (new_pos < 52)
            {
                target_pos = new_pos;
            }
            else
            {
                //wrap around
                target_pos = new_pos - 52;
            }
        }
        
        //check for game stuff
        if (isOccupied(target_pos)) //we are now back in absolute coordinates again
        {
            if (isGlobe(target_pos))
            {
                target_pos = -1; //send me home
            }
            else
            {
                send_them_home(target_pos);
            }
        }

        player_positions[fixed_piece] = target_pos;
    }
    std::vector<int> new_relative = relativePosition();
    return post_game_analysis(new_relative, order[color]);
//    bool res;
//    switch (color)
//    {
//        case 0:
//            res = post_game_analysis(new_relative, order[0]);
//            break;
//        case 1:
//            res = post_game_analysis(new_relative, order[1]);
//            break;
//        case 2:
//            res = post_game_analysis(new_relative, order[2]);
//            break;
//        case 3:
//            res = post_game_analysis(new_relative, order[3]);
//        default:
//            break;
//    }
//    return res;
}

std::vector<int> my_game::relativePosition()
{
    std::vector<int> relative_positons;
    int modifier = color * 13;

    //from start id to end
    for (int i = color*4; i < player_positions.size(); ++i)
    {
        relative_positons.push_back(player_positions[i]);
    }
    //from 0 to start id
    for (int i = 0; i < color*4; ++i)
    {
        relative_positons.push_back(player_positions[i]);
    }

    for (size_t i = 0; i < relative_positons.size(); ++i)
    {
        if (relative_positons[i] == 99 || relative_positons[i] == -1)
        {
            relative_positons[i] = (relative_positons[i]);
        }
        else if (relative_positons[i] < modifier)
        {
            relative_positons[i] = (relative_positons[i]+52-modifier);
        }
        else if (relative_positons[i] > /*50*/ 51) //original implementation is incorrect!
        {
            //relative_positons[i] = (relative_positons[i]-color*5-1);
            
            //proposed fix:
            int tmp = relative_positons[i] - 52;
            if (tmp < color*5)
            {
                tmp = (tmp + 20 - color*5);
            }
            else //if (tmp >= color*5)
            {
                tmp = (tmp - color*5);
            }
            relative_positons[i] = tmp + 51;
        }
        else if (relative_positons[i] >= modifier)
        {
            relative_positons[i] = (relative_positons[i]-modifier);
        }
    }
    return std::move(relative_positons);
}

void my_game::turnComplete(bool win)
{
    if (log_utils)
    {
        utils_buffer.push_back(CalcUtilities());
    }
    
    game_complete = win;
    turn_complete = true;
    if (game_complete)
    {
//        if (trainingPlayer != -1)
//        {
//            if (order[color] != trainingPlayer)
//            {
//                int org_color = color;
//                vector<int> rel;
//                for (int i = 0; i < 4; i++)
//                {
//                    color = i;
//                    if (order[color] == trainingPlayer)
//                        rel = relativePosition();
//                }
//                color = org_color;
                
//                SendEndState(trainingPlayer, rel); //notify the player we are training that it lost
//            }
//        }
        
        if (SendEndState != nullptr)
        {
            int org_color = color;
            vector<int> rel;
            for (int i = 0; i < 4; i++)
            {
                color = i;
                rel = relativePosition();
                SendEndState(order[color], rel); //notify the player of the games end state
            }
            color = org_color;
        }
        
        if (info)
        {
            cout << "player: " << order[color] << " won" << endl;
        }
        
        if (log_wins)
        {
            std::ofstream outputFile;
            outputFile.open(logfile_path + "wins.txt", std::ios::out | std::ios::app);
            if (outputFile.is_open())
            {
                outputFile << order[color] << "\n";
                outputFile.close();
            }
            else
            {
                cout << "couldn't open file!" << endl;
            }
        }
        
        if (log_utils)
        {
            for (int i = 0; i < 4; i++)
            {
                string name = logfile_path + "util_g" + game_number + "_p";
                if (order[i] == 1)
                    name += to_string(1);
                else if (order[i] == 2)
                    name += to_string(2);
                else if (order[i] == 3)
                    name += to_string(3);
                else //if (order[i] == 4)
                    name += to_string(4);
                
                ofstream outputFile;
                outputFile.open(name + ".txt", std::ios::out | std::ios::app);
                if (outputFile.is_open())
                {
                    for (int j = 0; j < utils_buffer.size(); j++)
                    {
                        outputFile << utils_buffer[j][i] << "\n";
                    }
                    outputFile.close();
                }
                else
                {
                    cout << "couldn't open file!" << endl;
                }
            }
        }
    }
}

int my_game::run()
{
    //int turns = 0;
    while (!game_complete)
    {
        if (turn_complete)
        {
            turn_complete = false;
            next_turn();
//            turns++;
//            cout << "turns: " << turns << endl;
        }
    }
    
    return order[color];
}

vector<int> my_game::CalcUtilities()
{
    int org_color = color;
    vector<int> res = {0, 0, 0, 0};
    for (int i = 0; i < 4; i++)
    {
        color = i;
        vector<int> rel = relativePosition();
        for (int j = 0; j < 4; j++)
        {
            int tmp;
            if (rel[j] == -1)
            {
                tmp = 0;
            }
            else if (rel[j] == 99)
            {
                tmp = 57;
            }
            else
            {
                tmp = rel[j] + 1;
            }
            res[i] += tmp;
        }
    }
    color = org_color;
    return res;
}
