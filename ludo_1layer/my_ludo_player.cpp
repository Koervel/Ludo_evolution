#include "my_ludo_player.h"



my_ludo_player::my_ludo_player()
{
    dist = uniform_int_distribution<int>(0, 99);
}

int my_ludo_player::make_decision()
{
    //find all legal moves
    std::vector<int> valid_moves;
    if(dice_roll == 6)
    {
        for(int i = 0; i < 4; ++i)
        {
            if(pos_start_of_turn[i]<0)
            {
                valid_moves.push_back(i);
            }
        }
    }
    
    for(int i = 0; i < 4; ++i)
    {
        if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99)
        {
            valid_moves.push_back(i);
        }
    }
    
    int chosen_action = -1;
    if (valid_moves.size() > 0) //if we can actually choose an action
    {
        if (dist(gen) < percent && learning) //exploration (epsilon-greedy)
        {
            //choose piece at random
            uniform_int_distribution<int> tmp_dist(0, valid_moves.size() - 1);
            int piece = tmp_dist(gen);
            
            chosen_action = valid_moves[piece];
        }
        else //exploitation
        {
            //find the best action
            double bestQ = -INFINITY;
            int bestAction;
            
            for (int i = 0; i < valid_moves.size(); i++) //for all possible actions
            {
                //calculate the Q-value
                double Q = network->CalcOutput(ToNetworkInput(pos_start_of_turn, dice_roll, valid_moves[i])).mat[0][0];
                
                //check if it's better
                if (Q >= bestQ)
                {
                    bestQ = Q;
                    bestAction = valid_moves[i];
                }
            }
            
            chosen_action = bestAction;
        }
        
        if (learning)
        {
            LearningData temp;
            temp.rel_pos = pos_start_of_turn;
            temp.dice = dice_roll;
            temp.action = valid_moves[0];
            temp.valid_moves = valid_moves;
            data.push_back(temp);
        }
    }
    
    //just do something if we have no legal moves
    if(valid_moves.size()==0)
    {
        for(int i = 0; i < 4; ++i)
        {
            if(pos_start_of_turn[i] != 99)
            {
                valid_moves.push_back(i);
            }
        }
    }
    
    //return the action
    if (chosen_action == -1)
        return valid_moves[0];
    else
        return chosen_action;
}

vector<double> my_ludo_player::RelPos2State(vector<int> rel_pos)
{
    vector<int> glob_pos = theLawsOfTheUniverse.Rel2Glob(rel_pos);
    return GlobPos2State(glob_pos);
}

vector<double> my_ludo_player::GlobPos2State(vector<int> glob_pos)
{
    vector<double> res(16, -1.0);
    vector<int> progress = theLawsOfTheUniverse.CalcProgress(glob_pos);
    for (int i = 0; i < glob_pos.size(); i++)
    {
        int tmp = progress[i];
        if (tmp > 55)
            tmp = 57;
        else
            tmp += 1;
        
        res[i] = ((double)tmp / 28.5) - 1.0; //normalise to the range [-1; 1]
    }
    return res;
}

Matrix my_ludo_player::ToNetworkInput(vector<int> a_rel_pos, int a_dice_roll, int an_action)
{
    Matrix res(21, 1);
    
    //positions
    vector<int> glob_pos = theLawsOfTheUniverse.Rel2Glob(a_rel_pos);
    vector<int> progress = theLawsOfTheUniverse.CalcProgress(glob_pos);
    for (int i = 0; i < 16; i++)
    {
        int tmp = progress[i];
        if (tmp > 55)
            tmp = 57;
        else
            tmp += 1;
        
        //res.mat[i][0] = ((double)tmp / 28.5) - 1.0; //normalise to the range [-1; 1]
        res.mat[i][0] = ((double)tmp / 57.0); //normalise to the range [0; 1]
    }
    
    //dice roll
    //res.mat[16][0] = ((double)a_dice_roll / 3.0) - 1.0; //normalise to the range [-1; 1]
    res.mat[16][0] = ((double)a_dice_roll / 6.0); //normalise to the range [0; 1]
    
    //actions
    for (int i = 0; i < 4; i++)
    {
        if (i == an_action)
            res.mat[16][0] = 1.0;
        else
            res.mat[16][0] = 0.0;
    }
    
    return res;
}

int my_ludo_player::start_turn(positions_and_dice relative)
{
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    return decision;
}

bool my_ludo_player::post_game_analysis(std::vector<int> relative_pos)
{
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i)
    {
        if(pos_end_of_turn[i] < 99)
        {
            game_complete = false;
        }
    }
    return game_complete;
}

void my_ludo_player::RecieveEndState(vector<int> rel_pos)
{
    if (learning)
    {
        LearningData temp;
        temp.rel_pos = rel_pos;
        temp.terminal = true;
        data.push_back(temp);
    }
}
