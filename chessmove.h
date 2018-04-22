///////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMMER: DANIEL FUCHS
/// CLASS/SECT: CS5400A - ARTIFICIAL INTELLIGENCE
/// ASSIGNMENT: CHESS GAME ASSIGNMENT: PART #4
/// DATE: 4/12/18
/// DESC: Header file for "Chessmove", a class that describes a chess move.
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
using namespace std;

#ifndef CHESSMOVE_H
#define CHESSMOVE_H

class Chessmove
{
public:
    int src_i;
    int dst_i;
    char piece;
    char movtype; // S (Standard), Q/R/B/N (Promotions), J (Pawn Two-Space Move), P (Enpassant). C (Castle)
    bool capture;

    //Default Constructor; Sets negative values, defaults standard move type.
    Chessmove(): src_i(-1), dst_i(-1), piece('?'), movtype('S'), capture(false) { };

    //Parameterized Constructor; Only requires Source index. Destination index and piece are optional.
    Chessmove(const int source, const char moving_piece = '?', const int destination = -1);

    //Copy Constructor; Standard Behaviour
    Chessmove(const Chessmove & rhs) = default;

    //Operator== Overload; Positional Equality Test
    bool operator==(const Chessmove & rhs)const;

    //Operator= Overload; Standard Behaviour
    Chessmove & operator=(const Chessmove & rhs) = default;

    friend ostream & operator<<(ostream & os, const Chessmove & rhs);
};

#endif
