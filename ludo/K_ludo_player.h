#ifndef K_LUDO_PLAYER_H
#define K_LUDO_PLAYER_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"
#include <QtGlobal>
#include <iostream>
#include <fstream>


using namespace std;
struct NN{
    vector<vector<double>> h1;
    vector<vector<double>> h2;
    vector<vector<double>> h3;
};


class K_ludo_player : public QObject {
    Q_OBJECT
private:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    int dice_roll;
    int make_decision();
    int pickAValidMove(std::vector<double>priorty);
    vector<double> calcPriorty();
public:
    int steps;
    NN network;
    void reset();
    void printNN(NN aNetwork);
    K_ludo_player();
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    int start_turn(positions_and_dice relative);
    bool post_game_analysis(std::vector<int> relative_pos);
    double sigmoid(double);
    void save(string fileName);
    void load(string filename);
};

#endif // LUDO_PLAYER_H
