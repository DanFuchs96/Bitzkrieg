///////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMMER: DANIEL FUCHS
/// CLASS/SECT: CS5400A - ARTIFICIAL INTELLIGENCE
/// ASSIGNMENT: CHESS GAME ASSIGNMENT: PART #4
/// DATE: 4/12/18
/// DESC: Header file for class "Bitzkrieg". Bitzkrieg is a bitboard-based AI designed to play
/// chess.
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include "bitboard.h"
#include "lookup.h"
#include "chessmove.h"
#include "chessnode.h"
using namespace std;

#ifndef BITZKRIEG_H
#define BITZKRIEG_H

class Bitzkrieg
{
private:
    char player;                 //Represents player color
    Bitboard current_state;      //Stores current board state
    ChessNode* root_node;        //Represents current game state, also serves as root node for state evaluation
    vector<Chessmove> pastMoves; //Last eight moves leading up to current state (0 being the most recent)

    int TLHTQSABDLMM_Max(ChessNode *ref_node, int depth, int qs_d, int alpha, int beta);     //Maximum Function
    int TLHTQSABDLMM_Min(ChessNode *ref_node, int depth, int qs_d, int alpha, int beta);     //Minimum Function
    Chessmove TLHTQSABDLMM_Recursive(ChessNode *root_node, const int depth, const int qs_d); //MM Recursive Caller

public:
    Bitzkrieg();                                                     //Default Constructor
    Bitzkrieg(const string & fen, const vector<Chessmove> & moves);  //Parameterized Constructor
    ~Bitzkrieg() { if(root_node != nullptr) delete root_node; }      //Destructor
    void execute(const Chessmove & action);                          //Performs move
    void display();                                                  //Displays state
    vector<Chessmove> ACTIONS();                                     //Generates all possible actions
    ChessNode* RESULTS(ChessNode* parent, const Chessmove & action); //Returns new child node
    int countPieces()const;                                          //Returns number of pieces
    static void printMoves(const vector<Chessmove> & move);          //Prints moves

    Chessmove TLHTQSID_ABDLMM_Algorithm(const int max_depth, const int qs_depth);
    //Time-Limited History-Table Quiescent Iterative-Deepening Alpha-Beta Pruning Depth-Limited Minimax Algorithm.
    //This function serves as my implementation of TLHTQSID-ABDLMM. It generates a tree based on the root node,
    //performs the algorithm, and returns the selected move to perform. Note, my implementation also uses
    //transposition tables.
};

#endif
