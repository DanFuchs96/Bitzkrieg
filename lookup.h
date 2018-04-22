///////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMMER: DANIEL FUCHS
/// CLASS/SECT: CS5400A - ARTIFICIAL INTELLIGENCE
/// ASSIGNMENT: CHESS GAME ASSIGNMENT: PART #4
/// DATE: 4/12/18
/// DESC: Header file for class "Lookup". This file contains all hardcoded lookup constants and
/// special declarations. The "Lookup" class provides a static function to initialize lookup tables
/// and a general utility function to print register values.
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <cmath>
#include <unordered_map>
#include <vector>

using namespace std;

#ifndef LOOKUP_H
#define LOOKUP_H

typedef uint64_t U64;

class TranspositionEntry
{
public:
    bool valid;
    int alpha;
    int beta;
    int value;
    int plys;
    TranspositionEntry(): valid(false), alpha(0), beta(0), value(0), plys(0) { };
    TranspositionEntry(int a, int b, int v, int p): valid(true), alpha(a), beta(b), value(v), plys(p) { };
};

enum AN_mapping {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

enum castle_type {
    CWK, CWQ, CBK, CBQ
};

enum piece_type {
    WK, WQ, WR, WB, WN, WP, BK, BQ, BR, BB, BN, BP
};

const U64 Xfull = 0xFFFFFFFFFFFFFFFF;
const U64 Xnone = 0x0000000000000000;
const U64 Xnorm = 0xFFFF00000000FFFF;
const U64 Xdiag = 0x8040201008040201;
const U64 Xantd = 0x0102040810204080;
const U64 fileA = 0x0101010101010101;
const U64 fileB = 0x0202020202020202;
const U64 fileC = 0x0404040404040404;
const U64 fileD = 0x0808080808080808;
const U64 fileE = 0x1010101010101010;
const U64 fileF = 0x2020202020202020;
const U64 fileG = 0x4040404040404040;
const U64 fileH = 0x8080808080808080;
const U64 rank1 = 0x00000000000000FF;
const U64 rank2 = 0x000000000000FF00;
const U64 rank7 = 0x00FF000000000000;
const U64 rank8 = 0xFF00000000000000;
const U64 light = 0x55AA55AA55AA55AA;
const U64 dark =  0xAA55AA55AA55AA55;
const U64 notFA = ~fileA;
const U64 notFB = ~fileB;
const U64 notFG = ~fileG;
const U64 notFH = ~fileH;
const U64 notR1 = ~rank1;
const U64 notR2 = ~rank2;
const U64 notR7 = ~rank7;
const U64 notR8 = ~rank8;
const U64 CASTLE_NATK_WQ = 0x000000000000001C;
const U64 CASTLE_OPEN_WQ = 0x000000000000000E;
const U64 CASTLE_NATK_WK = 0x0000000000000070;
const U64 CASTLE_OPEN_WK = 0x0000000000000060;
const U64 CASTLE_NATK_BQ = 0x1C00000000000000;
const U64 CASTLE_OPEN_BQ = 0x0E00000000000000;
const U64 CASTLE_NATK_BK = 0x7000000000000000;
const U64 CASTLE_OPEN_BK = 0x6000000000000000;
const string AN_string[64] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};

const int UTILITY_MAX = 100000;
const int UTILITY_MIN = -100000;
const int UTILITY_DRAW = -50000;
const int TIMESLOT_MIN = 1500000;              //1.5 seconds
const unsigned int PANIC_THRESHOLD = 45000000; //45 seconds
const int TIMESLOT_RATE = 5;
const unsigned int DEPTH_OFFSETS[7] = { 5, 6, 7, 8, 9, 12, 15 };
const int QS_INITIAL = 5;
const int QS_THRESH = 250;

extern int getFile[64];
extern int getRank[64];
extern U64 bitMask[64];
extern U64 fileMask[64];
extern U64 rankMask[64];
extern U64 diagMask[64];
extern U64 antdMask[64];
extern U64 rookMask[64];
extern U64 bishopMask[64];
extern U64 queenMask[64];
extern U64 knightMask[64];
extern U64 kingMask[64];
extern U64 zobristTable[12][64];
extern unordered_map<U64, int> HistoryTable;
extern unordered_map<U64, TranspositionEntry> TranspositionTable;

class Lookup
{
private:
    Lookup() = default;
    static double REMAINING_TIME;
    static double DYNAMIC_TIMER;
    static clock_t TIMESLOT_START;
    static unsigned int DYNAMIC_DEPTH;
    static unsigned int HISTORY_MAX;
    static unsigned int DYNAMIC_QS_DEPTH;

public:
    static void INIT();                                          //Initializes Lookup Tables
    static void UPDATE(double r_time, unsigned int r_pcs);       //Updates variables used in size/time management
    static void printReg(U64 board);                             //Prints passed register
    static bool TimeslotOver();                                  //Returns true if timeslot is closed
    static int MaximumDepth() { return DYNAMIC_DEPTH; };         //Accessor (Depth Limit)
    static int HistoryMax() { return HISTORY_MAX; };             //Accessor (History Max)
    static int QuiescentDepth() { return DYNAMIC_QS_DEPTH; };    //Accessor (QS Depth Limit)
    static U64 ZobristHash(const U64 *chessboard);               //Computes Zobrist Hash Key
    static void FlushHistoryTable();                             //Clears History Table
    static void UpdateHistoryTable(U64 zobrist_key);             //Updates History Table
    static void FlushTranspositionTable();                       //Clears Transposition Table
    static void UpdateTranspositionTable(U64 zobrist_key);       //Updates Transposition Table
    static bool InTranspositionTable(U64 zobrist_key, int plys); //Checks Transposition Table for key
    static void ArrangeNodes(vector< pair<int, int> > & nodes);  //Arranges Vector of Abstract Nodes
};

#endif
