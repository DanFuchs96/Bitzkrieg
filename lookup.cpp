///////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMMER: DANIEL FUCHS
/// CLASS/SECT: CS5400A - ARTIFICIAL INTELLIGENCE
/// ASSIGNMENT: CHESS GAME ASSIGNMENT: PART #4
/// DATE: 4/12/18
/// DESC: Class implementation file for class "Lookup". Also contains defined lookup tables.
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <cmath>
#include <climits>
#include <unordered_map>
#include "lookup.h"
using namespace std;

int getFile[64];
int getRank[64];
U64 bitMask[64];
U64 fileMask[64];
U64 rankMask[64];
U64 diagMask[64];
U64 antdMask[64];
U64 rookMask[64];
U64 bishopMask[64];
U64 queenMask[64];
U64 knightMask[64];
U64 kingMask[64];
U64 zobristTable[12][64];
unordered_map<U64, int> HistoryTable;
unordered_map<U64, TranspositionEntry> TranspositionTable;

double Lookup::REMAINING_TIME = 0;
double Lookup::DYNAMIC_TIMER = 0;
clock_t Lookup::TIMESLOT_START;
unsigned int Lookup::DYNAMIC_DEPTH = 0;
unsigned int Lookup::HISTORY_MAX = 0;
unsigned int Lookup::DYNAMIC_QS_DEPTH = 0;

void Lookup::INIT()
{
    Lookup::DYNAMIC_QS_DEPTH = QS_INITIAL;
    int index;
    int offset;
    U64 reg_stor;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            index = (8*i) + j;
            getFile[index] = j;
            getRank[index] = i;
            bitMask[index] = 1ULL << index;
            fileMask[index] = fileA << j;
            rankMask[index] = rank1 << (8*i);
            offset = 8*(i - j);
            diagMask[index] = (offset > 0 ? Xdiag << offset : Xdiag >> abs(offset));
            offset = 8*(i + j - 7);
            antdMask[index] = (offset > 0 ? Xantd << offset : Xantd >> abs(offset));
        }
    }
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            index = (8*i) + j;
            rookMask[index] = fileMask[index] | rankMask[index];
            bishopMask[index] = diagMask[index] | antdMask[index];
            queenMask[index] = rookMask[index] | bishopMask[index];
            knightMask[index] = (((bitMask[index] << 6) | (bitMask[index] >> 10)) & (notFG & notFH)) |
                                (((bitMask[index] << 15) | (bitMask[index] >> 17)) & notFH) |
                                (((bitMask[index] << 17) | (bitMask[index] >> 15)) & notFA) |
                                (((bitMask[index] << 10) | (bitMask[index] >> 6)) & (notFA & notFB));
            kingMask[index] = (((bitMask[index] << 7) | (bitMask[index] >> 1) | (bitMask[index] >> 9)) & ~fileH) |
                              (((bitMask[index] << 9) | (bitMask[index] << 1) | (bitMask[index] >> 7)) & ~fileA) |
                              ((bitMask[index] << 8) | (bitMask[index] >> 8));
            for (int k = 0; k < 12; k++)
            {
                reg_stor = 0ULL;
                for (int l = 0; l < 64; l++)
                {
                    if (rand()%2) reg_stor <<= 1;
                    else reg_stor = ((reg_stor << 1) | 1ULL);

                }
                zobristTable[k][index] = reg_stor;
            }
        }
    }
}

void Lookup::UPDATE(double r_time, unsigned int r_pcs)
{
    //This function is used to configured time/depth management variables. In general, as the game progresses, the
    //maximum time per turn slowly decreases, while the maximum depth increases in tiers based on how many pieces
    //remain. When the remaining time drops below 45 seconds, "Panic Mode" is triggered. During Panic Mode, time per
    //move is greatly shortened. If 75% of Panic Mode's time elapses and the game still has not finished, the depth
    //limit is greatly decreased as well. This is to ensure the game will complete in time.

    Lookup::TIMESLOT_START = clock(); //Log Turn Start
    Lookup::REMAINING_TIME = r_time;  //Store Remaining Time (Expects Milliseconds)

    //Timeslot Setting; Generally 10% of Remaining Time
    if(Lookup::REMAINING_TIME < PANIC_THRESHOLD) Lookup::DYNAMIC_TIMER = TIMESLOT_MIN; //Panic Mode Setting
    else Lookup::DYNAMIC_TIMER = ( Lookup::REMAINING_TIME * TIMESLOT_RATE ) / 100.0;   //Standard Settings

    if(r_pcs < 27) DYNAMIC_TIMER *= 1.5; //Mid-game Computation Boost

    //Convert Timeslot to Clock Ticks
    Lookup::DYNAMIC_TIMER *= ( CLOCKS_PER_SEC / 1000000.0 );
    Lookup::DYNAMIC_TIMER += (double)Lookup::TIMESLOT_START;

    //Depth Setting; Generally Depends on Remaining Piece Count
    if(Lookup::REMAINING_TIME > PANIC_THRESHOLD) //Standard Settings
    {
        Lookup::DYNAMIC_DEPTH = (r_pcs > 31 ? DEPTH_OFFSETS[0] :
                                 r_pcs > 28 ? DEPTH_OFFSETS[1] :
                                 r_pcs > 25 ? DEPTH_OFFSETS[2] :
                                 r_pcs > 22 ? DEPTH_OFFSETS[3] :
                                 r_pcs > 10 ? DEPTH_OFFSETS[4] :
                                 r_pcs > 5 ? DEPTH_OFFSETS[5] :
                                 DEPTH_OFFSETS[6]);
    }
    else if (Lookup::REMAINING_TIME > PANIC_THRESHOLD/4) Lookup::DYNAMIC_DEPTH = 3; //Panic Mode Settings
    else { Lookup::DYNAMIC_DEPTH = 1; Lookup::DYNAMIC_QS_DEPTH = 0; }
}

void Lookup::printReg(U64 board)
{
    for (int rank = 7; rank > -1; rank--)
    {
        for (int file = 0; file < 8; file++)
        {
            cout << (bool)(board & (1ULL << ((rank * 8) + file))) << " ";
        }
        cout << endl;
    }
}

bool Lookup::TimeslotOver()
{
    return (clock() > Lookup::DYNAMIC_TIMER);
}

U64 Lookup::ZobristHash(const U64 *chessboard)
{
    U64 gen_key = 0ULL;
    bool ref_bit = false;
    for (int index = 0; index < 64; index++)
    {
        for (int piece_id = 0; piece_id < 12; piece_id++)
        {
            ref_bit = (bool)((chessboard[piece_id] >> index) & 1ULL);
            if (ref_bit)
            {
                gen_key = gen_key ^ zobristTable[piece_id][index];
                piece_id = 12;
            }
        }
    }
    return gen_key;
}

void Lookup::FlushHistoryTable()
{
    HistoryTable.clear();
    Lookup::HISTORY_MAX = 0;
}

void Lookup::UpdateHistoryTable(U64 zobrist_key)
{
    int value = HistoryTable[zobrist_key];
    HistoryTable[zobrist_key] = value + 1;
    if (value == Lookup::HISTORY_MAX) Lookup::HISTORY_MAX++;
}

void Lookup::FlushTranspositionTable()
{
    TranspositionTable.clear();
}

void Lookup::UpdateTranspositionTable(U64 zobrist_key)
{
    if (TranspositionTable[zobrist_key].valid) return;
    TranspositionTable[zobrist_key] = TranspositionEntry(0, 0, 0, 0);
}

bool Lookup::InTranspositionTable(U64 zobrist_key, int plys)
{
    if (TranspositionTable[zobrist_key].valid)
    {
        return TranspositionTable[zobrist_key].plys <= plys; //Lower ply implies more accurate entry
    }
    else return false;
}

void Lookup::ArrangeNodes(vector<pair<int, int> > & nodes)
{
    pair<int,int> key;
    int size = (int)nodes.size();
    int offset = 0;
    for (int i = 1; i < size; i++)
    {
        key = nodes[i];
        offset = i - 1;
        while (offset > -1 && nodes[offset].first < key.first)
        {
            nodes[offset + 1] = nodes[offset];
            offset--;
        }
        nodes[offset + 1] = key;
    }
}
