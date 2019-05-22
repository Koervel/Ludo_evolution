#include "K_ludo_player.h"
#include <random>

K_ludo_player::K_ludo_player(){
    steps = 0;
}
void K_ludo_player::reset(){
    steps = 0;
}

int K_ludo_player::make_decision(){
    steps++;
    return pickAValidMove(calcPriorty());
}

int K_ludo_player::pickAValidMove(std::vector<double>priorty)
{
    struct indexAndPri{
        int index;
        double priorty;
    };
    //if dice == 6 all moves are valid, pick the higsted priorty
    std::vector<indexAndPri> valid_moves;
    indexAndPri temp;
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]<0){
                temp.index = i;
                temp.priorty = priorty[i];
                valid_moves.push_back(temp);
            }
        }
    }
    for(int i = 0; i < 4; ++i){
        if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
            temp.index = i;
            temp.priorty = priorty[i];
            valid_moves.push_back(temp);
        }
    }
    if(valid_moves.size()==0){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i] != 99){
                temp.index = i;
                temp.priorty = priorty[i];
                valid_moves.push_back(temp);
            }
        }
    }
    indexAndPri best = valid_moves[0];

    for (int i = 1; i < valid_moves.size(); i++)
    {
        if(best.priorty<valid_moves[i].priorty)
        {
            best = valid_moves[i];
        }
    }
    //cout << best.priorty << endl;

    return best.index;
}

int K_ludo_player::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    return decision;
}

bool K_ludo_player::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    return game_complete;
}

vector<double> K_ludo_player::calcPriorty(){
    vector<double> layer1;

    //printNN(network);

    layer1.resize(network.h2.size()-1);

    for (int i = 0; i < layer1.size();i++)
    {
        layer1[i] = 0;
    }
    vector<int> postion = pos_start_of_turn;
    postion.push_back(dice_roll);
    postion.push_back(1);

    for (int i = 0;i < layer1.size() ; i++)
    {
        for (int j = 0;j < postion.size() ; j++)
        {
            layer1[i] += postion[j] * network.h1[j][i];
           // cout << layer1[i] << " j = " << j << endl;

        }
        //cout << layer1[i] << " i =" << i << endl;
    }

    /*for (int i = 0; i < layer1.size();i++)
    {
        cout << layer1[i] << ";";
    }
    cout << "\n";*/
    for (int i = 0; i < layer1.size();i++)
    {
        layer1[i] = sigmoid(layer1[i]);
    }


    layer1.push_back(1);

    vector<double> ouput;
    ouput.resize(4);
    for (int i = 0;i < 4;i++)
    {
        ouput[i]= 0;
    }

    for (int i = 0;i < ouput.size() ; i++)
    {
        for (int j = 0;j < layer1.size() ; j++)
        {
            ouput[i] += layer1[j] * network.h2[j][i];
        }
    }
    for (int i = 0;i < 4;i++)
    {
        ouput[i]= sigmoid(ouput[i]);
    }


    /*for (int i = 0; i < ouput.size();i++)
    {
        cout << ouput[i] << ";";
    }
    cout << "\n";*/

    return ouput;

}

void K_ludo_player::printNN(NN aNetwork)
{
    cout << "H1" << endl;
    for (int i = 0; i < aNetwork.h1.size();i++) {
        for (int j = 0; j < aNetwork.h2.size();j++) {
            cout << aNetwork.h1[i][j] << ";";
        }
        cout << endl;
    }

    cout << "H2" << endl;
    for (int i = 0; i < aNetwork.h2.size();i++) {
        for (int j = 0; j < 4;j++) {
            cout << aNetwork.h2[i][j] << ";";
        }
        cout << endl;
    }

    /*cout << "H3" << endl;
    for (int i = 0; i < aNetwork.h3.size();i++) {
        for (int j = 0; j < 4;j++) {
            cout << aNetwork.h3[i][j] << ";";
        }
    cout << endl;
    }*/

}

double K_ludo_player::sigmoid(double aValue)
{
    double res;
    if (aValue < -5.0)
    {
        res = -1.0;
    }
    else if (aValue > 5.0)
    {
        res = 1.0;
    }
    else //only call exp() in the safe range, otherwise it may blow up and return nan!
    {
        double tmp1 = exp(aValue);
        double tmp2 = exp(-aValue);
        res = (tmp1 - tmp2) / (tmp1 + tmp2);
    }
    return res;
}

void K_ludo_player::save(string filename)
{
    std::ofstream outputFile;

    outputFile.open(filename + ".txt", ios::out);
    if(outputFile.is_open() == true)
    {
        for (int i = 0; i < network.h1.size();i++)
        {
            for (int j = 0; j < network.h2.size();j++)
            {
                outputFile << network.h1[i][j] << ";";
            }
            outputFile << endl;
        }
        outputFile << "##" << endl;

        for (int i = 0; i < network.h2.size();i++)
        {
            for (int j = 0; j < 4;j++)
            {
                outputFile << network.h2[i][j] << ";";
            }
            outputFile << endl;
        }

        outputFile << "##" << endl;
        outputFile.close();
    }
    else
    {
     cout << "error could not open file" << endl;
    }

}

void K_ludo_player::load(string filename)
{
    std::ifstream inputFile;
    string line;
    string temp;
    network.h1.resize(18,vector<double>(40));
    network.h2.resize(40,vector<double>(4));
    inputFile.open(filename + ".txt", ios::out);
    int row = 0;
    if(inputFile.is_open() == true)
    {
        for (int i = 0; i < 18; i++)
        {
           getline(inputFile,line);
           row = 0;
           for (int j = 0; j < line.size(); j++)
           {
                if(line[j] ==';')
                {
                    //cout << temp << endl;
                    double test = stold(temp);
                    //cout << test << endl;
                    network.h1[i][row] = test;
                    temp.resize(0);
                    row++;
                    //cout << network.h1[i][row] << endl;
                }
                else
                {
                    temp.push_back(line[j]);
                }
           }
        }
        getline(inputFile,line);
        if(line == "##")
        {
            row = 0;
            temp.resize(0);
            for (int i = 0; i < 20; i++)
            {
            getline(inputFile,line);
            row = 0;
            //cout << i << endl;
            for (int j = 0; j < line.size(); j++)
            {
                 //cout << j << endl;
                 if(line[j] ==';')
                 {
                     //cout << temp << endl;
                     double test = stold(temp);
                     //cout << test << endl;
                     network.h2[i][row] = test;
                     temp.resize(0);
                     row++;
                     //cout << network.h1[i][row] << endl;
                 }
                 else
                 {
                     temp.push_back(line[j]);
                 }
            }
            }
        }
        else
        {
         cout << "Error" << endl;
        }
    //printNN(network);

    }
    else
    {
     cout << "error could not open file" << endl;
    }

}
