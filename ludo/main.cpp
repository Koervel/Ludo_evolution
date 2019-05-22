//#include "dialog.h"
//#include <QApplication>
//#include "game.h"

#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "positions_and_dice.h"

#include "my_ludo_player.h"
#include "K_ludo_player.h"
#include <iostream>
#include "my_game.h"
#include <algorithm>
#include <iomanip>

#define RAND_MAX 50.0
#define MAX_CHANGE 20



using namespace std;

//Q_DECLARE_METATYPE( positions_and_dice )

struct PriortyAndNN{
    double priorty = 0;
    int won = 0;
    NN theNetwork;
};

bool compareInterval(PriortyAndNN *i1, PriortyAndNN *i2)
{
    return (i1->won > i2->won);
}



vector<int> SetPlayers(my_game& game, bool random);

void makeChild(PriortyAndNN* parentOne,PriortyAndNN* parentTwo, PriortyAndNN* child);

//the game's link to the players
int start_turn(positions_and_dice relative, int player);
bool post_game_analysis(std::vector<int> relative_pos, int player);

int start_turn2(positions_and_dice relative, int player);
bool post_game_analysis2(std::vector<int> relative_pos, int player);

void SendEndState_train(int player, vector<int> rel);

int start_turn_test(positions_and_dice relative, int player);
bool post_game_analysis_test(std::vector<int> relative_pos, int player);
NN buildRandomNN(int L1, int L2);

double Reward(vector<int> glob_pos);

//players for training
K_ludo_player p1;

//players for testing

ludo_player p2_test;
ludo_player p3_test;
ludo_player p4_test;

K_ludo_player p2;
K_ludo_player p3;
K_ludo_player p4;

struct QLearningData
{
    bool terminal = false;
    
    vector<int> rel_pos_pre;
    int dice_pre;
    int action;
    
    double reward;
    
    vector<int> rel_pos_res;
    int dice_res;
    vector<int> valid_moves;
};

int main(int argc, char *argv[])
{


    vector<PriortyAndNN*> ListOfNetworks(30);

    for (int i = 0;i < 30; i++)
    {
        PriortyAndNN *temp = new PriortyAndNN;
        temp->theNetwork = buildRandomNN(30,20);
        ListOfNetworks[i]= temp;
         //printNN(temp.theNetwork);
    }

    //game for training
    my_game train;
    train.start_turn = &start_turn;
    train.post_game_analysis = &post_game_analysis;
    train.order = {0, 1, 2, 3};
    
    
    //train
    bool random = true;
    int games_train = 1000;
    int skip = 0;
    double pre = 0;
    
    //vector<int> order = SetPlayers(train, random);
    
    GameRules theLawsOfTheUniverse;
    random_device rd;
    int winner;
    for (int i = 0; i < games_train; i++)
    {
        //do a g
        for (int j = 0; j < ListOfNetworks.size();j++)
        {
            p1.network = ListOfNetworks[j]->theNetwork;

           if(ListOfNetworks[j]->priorty==0)
           {
                for (int k = 0; k < 100; k++)
                {
                winner = train.run();
                if(winner == 0)
                {
                    ListOfNetworks[j]->priorty += p1.steps*0.1;
                    ListOfNetworks[j]->won++;
                }
                else
                {
                    ListOfNetworks[j]->priorty += p1.steps*20;
                }

                train.reset();
                p1.reset();
                }
            }
        }

        sort(ListOfNetworks.begin(),ListOfNetworks.end(),compareInterval);
        int index = 0;
        double avgWinrate = 0;
        for (int j = 0; j < ListOfNetworks.size();j++)
        {
            avgWinrate += (ListOfNetworks[j]->won/100.0);
        }

        cout <<"[30,20] At gen " << i << " best win rate " << (ListOfNetworks[0]->won/100.0) << ", The avg winrate "  << (avgWinrate/30.0) << endl;

        for (int i = 0; i < 18 ; i+=2)
        {
            //cout << 25+i << " : " << 27+i << endl;
            makeChild(ListOfNetworks[index],ListOfNetworks[index+1],ListOfNetworks[10+i]);
            makeChild(ListOfNetworks[index],ListOfNetworks[index+1],ListOfNetworks[11+i]);
            index++;
         }
         makeChild(ListOfNetworks[0],ListOfNetworks[9],ListOfNetworks[28]);
         makeChild(ListOfNetworks[0],ListOfNetworks[9],ListOfNetworks[29]);

         /*if(pre == 0 || pre != ListOfNetworks[0]->priorty)
         {
             pre = ListOfNetworks[0]->priorty;
             skip = 0;
         }
         skip++;
         if(skip == 100)
         {
             break;
         }

         if(i%100==0)
         {
             cout << "Saving best player" << endl;
             p1.network = ListOfNetworks[0]->theNetwork;
             p1.save("LudoPlayerTrain_20_20"+to_string(i)+"_"+to_string(ListOfNetworks[0]->won/100.0));
             for (int i = 0; i < ListOfNetworks.size();i++)
             {
             ListOfNetworks[i]->won = 0;
             ListOfNetworks[i]->priorty = 0;
             }
         }*/

    }
    cout << "Done! wtih Ludo Player Saving top" << endl;
    p1.network = ListOfNetworks[0]->theNetwork;
    p1.save("LudoPlayerTrainDone_30_20");
    //__________________________________________________________Train agianst onself -------------------------------------------

   /* my_game trainOnself;
    trainOnself.start_turn = &start_turn2;
    trainOnself.post_game_analysis = &post_game_analysis2;
    trainOnself.order = {0, 1, 2, 3};
    for (int n = 0; n < 20 ; n++)
    {

    if(ListOfNetworks[0]->won >= 25)
    {
        for (int i = 0; i < ListOfNetworks.size();i++)
        {
        ListOfNetworks[i]->won = 0;
        ListOfNetworks[i]->priorty = 0;
        }

        NN theCurrentBest = ListOfNetworks[0]->theNetwork;
        p2.network = theCurrentBest;
        p3.network = theCurrentBest;
        p4.network = theCurrentBest;
        cout << "updating network" << endl;
    }

    for (int i = 0; i < 50; i++)
    {
        //do a g
        for (int j = 0; j < ListOfNetworks.size();j++)
        {
            p1.network = ListOfNetworks[j]->theNetwork;

           if(ListOfNetworks[j]->priorty==0)
           {
                for (int k = 0; k < 100; k++)
                {
                winner = trainOnself.run();
                if(winner == 0)
                {
                    ListOfNetworks[j]->priorty += p1.steps*0.1;
                    ListOfNetworks[j]->won++;
                }
                else
                {
                    ListOfNetworks[j]->priorty += p1.steps*20;
                }

                trainOnself.reset();
                p1.reset();
                p2.reset();
                p3.reset();
                p4.reset();
                }
            }
        }

        sort(ListOfNetworks.begin(),ListOfNetworks.end(),compareInterval);
        int index = 0;
        double avgWinrate = 0;
        for (int j = 0; j < ListOfNetworks.size();j++)
        {
            avgWinrate += (ListOfNetworks[j]->won/100.0);
        }

        cout <<"[20,20] At gen " << i << " best win rate " << (ListOfNetworks[0]->won/100.0) << ", The avg winrate "  << (avgWinrate/30.0) << endl;

        for (int i = 0; i < 18 ; i+=2)
        {
            //cout << 25+i << " : " << 27+i << endl;
            makeChild(ListOfNetworks[index],ListOfNetworks[index+1],ListOfNetworks[10+i]);
            makeChild(ListOfNetworks[index],ListOfNetworks[index+1],ListOfNetworks[11+i]);
            index++;
         }
         makeChild(ListOfNetworks[0],ListOfNetworks[9],ListOfNetworks[28]);
         makeChild(ListOfNetworks[0],ListOfNetworks[9],ListOfNetworks[29]);

    }
    cout << "Done! wtih lvl 2 Saving top " + to_string(n) << endl;
    p1.network = ListOfNetworks[0]->theNetwork;
    p1.save("TrainLv2_20_20_"+to_string(n)+"_"+to_string(ListOfNetworks[0]->won/100.0));
    }*/

    
    return 0;
}

vector<int> SetPlayers(my_game& game, bool random)
{
    if (random)
    {
        vector<int> tmp = {0, 1, 2, 3};
        random_device rd;
        shuffle(tmp.begin(), tmp.end(), rd);
        
        game.order = tmp;
        return tmp;
    }
    else
    {
        game.order = {0, 1, 2, 3};
        return {0, 1, 2, 3};
    }
}

/*int start_turn_train(positions_and_dice relative, int player)
{
//    cout << "player " << player << " start turn!" << endl;
//    cout << "dice: " << relative.dice << endl;
    if (player == 0)
        return p1.start_turn(relative);
    else if (player == 1)
        return p2_train.start_turn(relative);
    else if (player == 2)
        return p3_train.start_turn(relative);
    else //if (player == 3)
        return p4_train.start_turn(relative);
}

bool post_game_analysis_train(std::vector<int> relative_pos, int player)
{
    if (player == 0)
        return p1_train.post_game_analysis(relative_pos);
    else if (player == 1)
        return p2_train.post_game_analysis(relative_pos);
    else if (player == 2)
        return p3_train.post_game_analysis(relative_pos);
    else //if (player == 3)
        return p4_train.post_game_analysis(relative_pos);
}

void SendEndState_train(int player, vector<int> rel)
{
    if (player == 0)
        p1_train.RecieveEndState(rel);
    else if (player == 1)
        p2_train.RecieveEndState(rel);
    else if (player == 2)
        p3_train.RecieveEndState(rel);
    else if (player == 3)
        p4_train.RecieveEndState(rel);
}

int start_turn_test(positions_and_dice relative, int player)
{
    if (player == 0)
        return p1_test.start_turn(relative);
    else if (player == 1)
        return p2_test.start_turn(relative);
    else if (player == 2)
        return p3_test.start_turn(relative);
    else //if (player == 3)
        return p4_test.start_turn(relative);
}*/

int start_turn(positions_and_dice relative, int player)
{
    if (player == 0)
        return p1.start_turn(relative);
    else if (player == 1)
        return p2_test.start_turn(relative);
    else if (player == 2)
        return p3_test.start_turn(relative);
    else //if (player == 3)
        return p4_test.start_turn(relative);
}

bool post_game_analysis(std::vector<int> relative_pos, int player)
{
    if (player == 0)
        return p1.post_game_analysis(relative_pos);
    else if (player == 1)
        return p2_test.post_game_analysis(relative_pos);
    else if (player == 2)
        return p3_test.post_game_analysis(relative_pos);
    else //if (player == 3)
        return p4_test.post_game_analysis(relative_pos);
}

int start_turn2(positions_and_dice relative, int player)
{
    if (player == 0)
        return p1.start_turn(relative);
    else if (player == 1)
        return p2.start_turn(relative);
    else if (player == 2)
        return p3.start_turn(relative);
    else //if (player == 3)
        return p4.start_turn(relative);
}

bool post_game_analysis2(std::vector<int> relative_pos, int player)
{
    if (player == 0)
        return p1.post_game_analysis(relative_pos);
    else if (player == 1)
        return p2.post_game_analysis(relative_pos);
    else if (player == 2)
        return p3.post_game_analysis(relative_pos);
    else //if (player == 3)
        return p4.post_game_analysis(relative_pos);
}

double Reward(vector<int> glob_pos)
{
//    vector<int> progress_prev = theLawsOfTheUniverse.CalcProgress(prev);
//    for (int i = 0; i < progress_prev.size(); i++)
//    {
//        int tmp = progress_prev[i];
//        if (tmp > 55)
//            tmp = 57;
//        else
//            tmp += 1;
        
//        progress_prev[i] = tmp;
//    }
    
//    double U_state = 0.0;
//    for (int i = 0; i < 4; i++)
//        U_state += (double)progress_prev[i];
    
//    double temp = 0.0;
//    for (int i = 4; i < 16; i++)
//        temp += (double)progress_prev[i];
//    U_state -= temp / 3.0;
    
//    vector<int> progress_curr = theLawsOfTheUniverse.CalcProgress(curr);
//    for (int i = 0; i < progress_curr.size(); i++)
//    {
//        int tmp = progress_curr[i];
//        if (tmp > 55)
//            tmp = 57;
//        else
//            tmp += 1;
        
//        progress_curr[i] = tmp;
//    }
    
//    double U_statePrime = 0.0;
//    for (int i = 0; i < 4; i++)
//        U_statePrime += (double)progress_curr[i];
    
//    double temp = 0.0;
//    for (int i = 4; i < 16; i++)
//        temp += (double)progress_curr[i];
//    U_statePrime -= temp / 3.0;
    
//    //cout << "reward: " << U_statePrime << endl;
    
//    //return U_statePrime - U_state;
    
//    if (curr[0] == 99 && curr[1] == 99 && curr[2] == 99 && curr[3] == 99) //we assume we are always green (and we are in this framework)
//        return U_statePrime;
//    else if (curr[4] == 99 && curr[5] == 99 && curr[6] == 99 && curr[7] == 99)
//        return U_statePrime;
//    else if (curr[8] == 99 && curr[9] == 99 && curr[10] == 99 && curr[11] == 99)
//        return U_statePrime;
//    else if (curr[12] == 99 && curr[13] == 99 && curr[14] == 99 && curr[15] == 99)
//        return U_statePrime;
//    else
//        return 0.0;
    
    if (glob_pos[0] == 99 && glob_pos[1] == 99 && glob_pos[2] == 99 && glob_pos[3] == 99) //we assume we are always green (and we are in this framework)
        return 1.0;
    else if (glob_pos[4] == 99 && glob_pos[5] == 99 && glob_pos[6] == 99 && glob_pos[7] == 99)
        return -1.0;
    else if (glob_pos[8] == 99 && glob_pos[9] == 99 && glob_pos[10] == 99 && glob_pos[11] == 99)
        return -1.0;
    else if (glob_pos[12] == 99 && glob_pos[13] == 99 && glob_pos[14] == 99 && glob_pos[15] == 99)
        return -1.0;
    else
        return 0.0;
    
//    if (curr[0] == 99 && prev[0] != 99)
//        return 50.0;
//    else if (curr[1] == 99 && prev[1] != 99)
//        return 50.0;
//    else if (curr[2] == 99 && prev[2] != 99)
//        return 50.0;
//    else if (curr[3] == 99 && prev[3] != 99)
//        return 50.0;
//    else
//        return -1.0;
}

void makeChild(PriortyAndNN* parentOne,PriortyAndNN* parentTwo, PriortyAndNN* child)
{
    std::default_random_engine rng;
    std::normal_distribution<double> ratio(1.0, 0.5);
    std::normal_distribution<double> constant(0.0,5);
    child->priorty = 0;
    child->won = 0;
    double temp;
    for (int i = 0; i < parentOne->theNetwork.h1.size();i++) {
        for (int j = 0; j < parentOne->theNetwork.h2.size();j++) {
            if(rand()%2==0)
            {
                child->theNetwork.h1[i][j] = parentOne->theNetwork.h1[i][j];
            }
            else
            {
                child->theNetwork.h1[i][j] = parentTwo->theNetwork.h1[i][j];
            }
            temp = (child->theNetwork.h1[i][j]*ratio(rng))+constant(rng);
            if(abs(temp)>0.0001)
            {
                if(abs(temp)<10000)
                {
                    child->theNetwork.h1[i][j] = (child->theNetwork.h1[i][j]*ratio(rng))+constant(rng);
                }
            }
            else {
                child->theNetwork.h1[i][j] = ratio(rng);
            }


        }
    }


    for (int i = 0; i < parentOne->theNetwork.h2.size();i++) {
        for (int j = 0; j < 4;j++) {
            if(rand()%2==0)
            {
                child->theNetwork.h2[i][j] = parentOne->theNetwork.h2[i][j];
            }
            else
            {
                child->theNetwork.h2[i][j] = parentTwo->theNetwork.h2[i][j];
            }

            temp = (child->theNetwork.h2[i][j]*ratio(rng))+constant(rng);
            if(abs(temp)>0.0001)
            {
                if(abs(temp)<10000)
                {
                    child->theNetwork.h2[i][j] = temp;
                }
            }
            else {
                child->theNetwork.h2[i][j] = ratio(rng);
            }
        }
    }

    for (int i = 0; i < parentOne->theNetwork.h3.size();i++) {
        for (int j = 0; j < 4;j++) {
            if(rand()%2==0)
            {
                child->theNetwork.h3[i][j] = parentOne->theNetwork.h3[i][j];
            }
            else
            {
                child->theNetwork.h3[i][j] = parentTwo->theNetwork.h3[i][j];
            }

            temp = (child->theNetwork.h3[i][j]*ratio(rng))+constant(rng);
            if(abs(temp)>0.0001)
            {
                if(abs(temp)<10000)
                {
                    child->theNetwork.h3[i][j] = temp;
                }
            }
            else {
                child->theNetwork.h3[i][j] = ratio(rng);
            }
        }
    }
}

NN buildRandomNN(int L1, int L2)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<> gen(-RAND_MAX, RAND_MAX);
    vector<vector<double>> ph1;
    NN toReturn;
    ph1.resize(18,vector<double>(L1+1)); //bias + dice + postion of players
    for (int i = 0;i < ph1.size();i++)
    {
        for (int j = 0; j < L1+1; j++)
        {
            ph1[i][j] = gen(rng);
        }
    }
    toReturn.h1 =ph1;

    vector<vector<double>> ph2;
    ph2.resize(L1+1,vector<double>(L2+1));
    for (int i = 0;i < ph2.size();i++)
    {
        for (int j = 0; j < L2+1; j++)
        {
            ph2[i][j] = gen(rng);
        }
    }

    toReturn.h2 = ph2;

    vector<vector<double>> ph3;
    ph3.resize(L2+1,vector<double>(4));
    for (int i = 0;i < ph3.size();i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ph3[i][j] = gen(rng);
        }
    }

    toReturn.h3 = ph2;

    return toReturn;
}
