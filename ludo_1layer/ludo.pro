#-------------------------------------------------
#
# Project created by QtCreator 2016-03-15T10:40:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ludo
TEMPLATE = app


SOURCES += main.cpp\
    dialog.cpp \
    game.cpp \
    ludo_player.cpp \
    ludo_player_random.cpp\
    my_ludo_player.cpp\
    K_ludo_player.cpp\
    my_game.cpp \
    neuralnetwork.cpp \
    gamerules.cpp

HEADERS  += dialog.h \
    game.h \
    ludo_player.h \
    positions_and_dice.h \
    ludo_player_random.h\
    my_ludo_player.h\
    K_ludo_player.h\
    my_game.h

FORMS    += dialog.ui

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wunused
