///////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMMER: DANIEL FUCHS
/// CLASS/SECT: CS5400A - ARTIFICIAL INTELLIGENCE
/// ASSIGNMENT: CHESS GAME ASSIGNMENT: PART #4
/// DATE: 4/12/18
/// DESC: Header file for class "Bitboard". This class handles chess logic, as well as move
/// generation and application.
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <cmath>
#include "lookup.h"
#include "chessmove.h"
using namespace std;

#ifndef BITBOARD_H
#define BITBOARD_H

class ChessNode;

enum GameStatus
{
    Ongoing, Stalemate, Checkmate
};

class Bitboard
{
private:
    U64 chessboard[12];  //Chess State
    char acting_color;   //Current player's turn
    bool WK_castle;      //If White King can castle king-side
    bool WQ_castle;      //If White King can castle queen-side
    bool BK_castle;      //If Black King can castle king-side
    bool BQ_castle;      //If Black King can castle queen-side
    bool self_check;     //Stores if acting player is in check
    int en_passant;      //Stores index of en_passant target
    int half_move_clock; //Half move counter
    int move_clock;      //Counts moves
    U64 zobristKey;      //Zobrist Hashing Key

    U64 mirrorBitboard(U64 src);           //Flips bitboard over y-axis.
    bool readBit(U64 src, int index)const; //Returns true if bit index in src is set to 1.
    int countBits(U64 src)const;           //Counts number of set bits in a register.
    int getLSBIndex(U64 src)const;         //Gets index of least significant bit in register. Expects non-zero value.
    U64 white()const;                      //Returns bitboard of all white occupations
    U64 black()const;                      //Returns bitboard of all black occupations
    U64 occupied()const;                   //Returns bitboard of all occupancies
    U64 empty()const;                      //Returns bitboard of open spaces
    U64 slide_EAST(U64 cardinal_slider, U64 occupancy = Xfull)const;  //Cardinal Slide East (Occluded Method)
    U64 slide_NORTH(U64 cardinal_slider, U64 occupancy = Xfull)const; //Cardinal Slide North
    U64 slide_WEST(U64 cardinal_slider, U64 occupancy = Xfull)const;  //Cardinal Slide West
    U64 slide_SOUTH(U64 cardinal_slider, U64 occupancy = Xfull)const; //Cardinal Slide South
    U64 slide_NE(U64 diagonal_slider, U64 occupancy = Xfull)const;    //Diagonal Slide Northeast
    U64 slide_NW(U64 diagonal_slider, U64 occupancy = Xfull)const;    //Diagonal Slide Northwest
    U64 slide_SW(U64 diagonal_slider, U64 occupancy = Xfull)const;    //Diagonal Slide Southwest
    U64 slide_SE(U64 diagonal_slider, U64 occupancy = Xfull)const;    //Diagonal Slide Southeast
    U64 attacked(char attacker)const;                                 //Computes all possible attacks of enemy player
    bool inCheck()const;                                              //Returns true if currently in check
    bool enChecked( Chessmove & action);                              //Returns true if action leads self into check
    bool isValidCastle(castle_type c_t, U64 attacked_region)const;    //Returns true if selected castle allowed.
    //Note that enChecked() is responsible for determining if a move is a capture.

public:
    Bitboard();                                    //Default Constructor; zeroes all bitboard registers.
    Bitboard(const Bitboard & src);                //Copy Constructor; copies all values, including state flags.
    Bitboard & operator=(const Bitboard & rhs);    //Operator= Overload; copies all values, including state flags.
    char getColor()const { return acting_color; }; //Accessor function for acting_color.
    int numTurn()const { return move_clock; };     //Accessor function for move_clock.
    U64 getZobrist()const { return zobristKey; };  //Accessor function for zobristKey.
    char readPiece(int index)const;                //Determine piece at index
    void display()const;                           //Prints out board
    int countPieces()const;                        //Counts number of pieces
    int evaluate(char attacker)const;              //Evaluates quality of current state (heuristic)
    GameStatus genMoves(vector<Chessmove> & validMoves); //Stores all possible moves into validMoves, gets status
    void performAction(const Chessmove & action);        //Executes an action, completing current player's turn.
    void loadFEN(const string & fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); //Load FEN

    friend class ChessNode; //Additional board manipulation / state evaluation is performed by the ChessNode class.
};

#endif