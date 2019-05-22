#include <vector>

using namespace std;



struct GameRules
{
    //-----------------------------------
    // Game logic (copied from the game)
    //-----------------------------------
    int dice_result;
    int color = 0; //we assume the player is green (we have no better knowledge and it simplifies things)
    vector<int> player_positions;
    
    int rel_to_fixed(int relative_piece_index)
    {
        return relative_piece_index + color * 4;
    }
    
    int isStar(int index)
    {
        if (index == 5  || index == 18 || index == 31 || index == 44)
            return 6;
        else if (index == 11 || index == 24 || index == 37 || index == 50)
            return 7;
        return 0;
    }
    
    int isOccupied(int index)
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
    
    bool isGlobe(int index)
    {
        if (index < 52)
            if (index % 13 == 0 || (index - 8) % 13 == 0 || isOccupied(index) > 1)
                return true;
        return false;
    }
    
    void send_them_home(int index)
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
    
    void move_start(int fixed_piece)
    {
        if (dice_result == 6 && player_positions[fixed_piece] < 0)
        {
            player_positions[fixed_piece] = color*13; //move me to start
            send_them_home(color*13); //send pieces home if they are on our start
        }
    }
    
    void movePiece(int relative_piece)
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
    }
    
    vector<int> relativePosition()
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
    
    //--------------
    // My functions
    //--------------
    vector<int> Rel2Glob(vector<int> relative_positions) //convert relative to global coordinates assuming the player is green
    {
        vector<int> res(16, 0);
        for (int i = 0; i < res.size(); i++)
        {
            if (relative_positions[i] == 99 || relative_positions[i] == -1)
            {
                res[i] = relative_positions[i];
            }
            else if (relative_positions[i] < 51)
            {
                res[i] = relative_positions[i];
            }
            else if (relative_positions[i] == 51)
            {
                if (i < 4)
                    res[i] = 52;
                else
                    res[i] = 51;
            }
            else if (relative_positions[i] > 51)
            {
                res[i] = relative_positions[i] + 1;
            }
        }
        return res;
    }
    
//    vector<int> CalcResRel(vector<int> relative_positions, int relative_piece, int dice_roll)
//    {
//        player_positions = Rel2Glob(relative_positions);
//        dice_result = dice_roll;
//        movePiece(relative_piece);
//        return relativePosition();
//    }
    
    vector<int> CalcResGlob(vector<int> relative_positions, int relative_piece, int dice_roll)
    {
        player_positions = Rel2Glob(relative_positions);
        dice_result = dice_roll;
        movePiece(relative_piece);
        return player_positions;
    }
    
    vector<int> CalcProgress(vector<int> glob_pos)
    {
        vector<int> res(glob_pos.size(), 0);
        for (int i = 0; i < glob_pos.size(); i++)
        {
            int local_color = 0;
            if (3 < i && i < 8)
                local_color = 1;
            else if (7 < i && i < 12)
                local_color = 2;
            else if (11 < i && i < 16)
                local_color = 3;
            
            int modifier = 13 * local_color;
        
            if (glob_pos[i] == 99 || glob_pos[i] == -1)
            {
                res[i] = glob_pos[i];
            }
            else if (glob_pos[i] < modifier)
            {
                res[i] = glob_pos[i] + 52 - modifier;
            }
            else if (glob_pos[i] > /*50*/ 51) //original implementation is incorrect!
            {
                //relative_positons[i] = (relative_positons[i]-color*5-1);
                
                //proposed fix:
                int tmp = glob_pos[i] - 52;
                if (tmp < local_color*5)
                {
                    tmp = (tmp + 20 - local_color*5);
                }
                else //if (tmp >= color*5)
                {
                    tmp = (tmp - local_color*5);
                }
                res[i] = tmp + 51;
            }
            else if (glob_pos[i] >= modifier)
            {
                res[i] = glob_pos[i] - modifier;
            }
        }
        return res;
    }
};
