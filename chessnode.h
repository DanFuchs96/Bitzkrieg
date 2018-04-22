///////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMMER: DANIEL FUCHS
/// CLASS/SECT: CS5400A - ARTIFICIAL INTELLIGENCE
/// ASSIGNMENT: CHESS GAME ASSIGNMENT: PART #4
/// DATE: 4/12/18
/// DESC: Header file for class "ChessNode". This class adds additional state-checking features to
/// an instance of a bitboard. It also allows for viewing the current conditions of the game as a
/// whole.
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <deque>
#include "bitboard.h"
#include "lookup.h"
#include "chessmove.h"
using namespace std;

#ifndef CHESSNODE_H
#define CHESSNODE_H

enum State_Condition
{
    Nonterminal, Loss, Draw_50, Draw_Rep, Draw_IM, Draw_Stale, Victory
};

class ChessNode
{
private:
    static Chessmove pastMoves[8]; //Past Eight Moves in reference to root state
    static char operating_player;  //Acting player in reference to root state
    Bitboard state;                //Local board state
    State_Condition condition;     //Current board's condition
    int heuristic_value;           //State/Utility Evaluation of current board
    int quiescence;                //Quiescent Level; Greater -> "Noisier" State
    int plys;                      //Number of moves ahead of current state
    Chessmove history[8];          //Local board move history
    vector<Chessmove> actions;     //Available moves from current board

    void setHistory(const Chessmove past[]);                             //Set local history
    void updateHistory(const Chessmove & action, const Chessmove old[]); //Add move to local history
    void setCondition();                                                 //Determine condition after computing moves
    void evaluateState();                                                //State Evaluation / Utility Computation

public:
    ChessNode() = delete; //Default Construction disallowed
    ChessNode(const Bitboard & board, const vector<Chessmove> & moves); //Root Constructor; builds base tree node
    ChessNode(const ChessNode & parent, const Chessmove & action); //Child Constructor; derives child from an action
    ChessNode(const ChessNode & src) = default;                    //Copy Constructor
    int heuristic()const { return heuristic_value; };              //Heuristic Value Accessor
    int numPlys()const { return plys; }                            //Plys Accessor
    const State_Condition status()const { return condition; };     //Condition Accessor
    const vector<Chessmove> & ACTIONS()const { return actions; }   //Action Accessor
    U64 zobristKey()const { return state.getZobrist(); }           //Zobrist Key Accessor
    bool isNotQuiescent() { return quiescence > QS_THRESH; }       //Returns true if state non-quiescent
    void display()const;                                           //Display Function
};

#endif
