///////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMMER: DANIEL FUCHS
/// CLASS/SECT: CS5400A - ARTIFICIAL INTELLIGENCE
/// ASSIGNMENT: CHESS GAME ASSIGNMENT: PART #4
/// DATE: 4/12/18
/// DESC: Class implementation file for class "Bitboard".
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <cmath>
#include "lookup.h"
#include "chessmove.h"
#include "bitboard.h"
using namespace std;

U64 Bitboard::mirrorBitboard(U64 src)
{
    return ((src & fileA) << 7) |
           ((src & fileB) << 5) |
           ((src & fileC) << 3) |
           ((src & fileD) << 1) |
           ((src & fileE) >> 1) |
           ((src & fileF) >> 3) |
           ((src & fileG) >> 5) |
           ((src & fileH) >> 7);
}

bool Bitboard::readBit(U64 src, int index)const
{
    return (bool)((src >> index) & 1ULL);
}

int Bitboard::countBits(U64 src)const
{
    int count = 0;
    while(src)
    {
        src &= src - 1ULL;
        count++;
    }
    return count;
}

int Bitboard::getLSBIndex(U64 src)const
{
    if(!src) { cout << "Warning, attempted to find LSB on zero-register. " << endl; return 64; }
    else return (int)log2(src & -src);
}

U64 Bitboard::white()const
{
    return (chessboard[WK] | chessboard[WQ] | chessboard[WR] |
            chessboard[WB] | chessboard[WN] | chessboard[WP]);
}

U64 Bitboard::black()const
{
    return (chessboard[BK] | chessboard[BQ] | chessboard[BR] |
            chessboard[BB] | chessboard[BN] | chessboard[BP]);
}

U64 Bitboard::occupied()const
{
    return (white() | black());
}

U64 Bitboard::empty()const
{
    return ~(occupied());
}

U64 Bitboard::slide_EAST(U64 cardinal_slider, U64 occupancy)const {
    occupancy &= notFA;
    cardinal_slider |= occupancy & (cardinal_slider << 1);
    occupancy &= (occupancy << 1);
    cardinal_slider |= occupancy & (cardinal_slider << 2);
    occupancy &= (occupancy << 2);
    cardinal_slider |= occupancy & (cardinal_slider << 4);
    return ((cardinal_slider << 1) & notFA);
}

U64 Bitboard::slide_NORTH(U64 cardinal_slider, U64 occupancy)const {
    cardinal_slider |= occupancy & (cardinal_slider << 8);
    occupancy &= (occupancy << 8);
    cardinal_slider |= occupancy & (cardinal_slider << 16);
    occupancy &= (occupancy << 16);
    cardinal_slider |= occupancy & (cardinal_slider << 32);
    return (cardinal_slider << 8);
}

U64 Bitboard::slide_WEST(U64 cardinal_slider, U64 occupancy)const {
    occupancy &= notFH;
    cardinal_slider |= occupancy & (cardinal_slider >> 1);
    occupancy &= (occupancy >> 1);
    cardinal_slider |= occupancy & (cardinal_slider >> 2);
    occupancy &= (occupancy >> 2);
    cardinal_slider |= occupancy & (cardinal_slider >> 4);
    return ((cardinal_slider >> 1) & notFH);
}

U64 Bitboard::slide_SOUTH(U64 cardinal_slider, U64 occupancy)const {
    cardinal_slider |= occupancy & (cardinal_slider >> 8);
    occupancy &= (occupancy >> 8);
    cardinal_slider |= occupancy & (cardinal_slider >> 16);
    occupancy &= (occupancy >> 16);
    cardinal_slider |= occupancy & (cardinal_slider >> 32);
    return (cardinal_slider >> 8);
}

U64 Bitboard::slide_NE(U64 diagonal_slider, U64 occupancy)const {
    occupancy &= notFA;
    diagonal_slider |= occupancy & (diagonal_slider << 9);
    occupancy &= (occupancy << 9);
    diagonal_slider |= occupancy & (diagonal_slider << 18);
    occupancy &= (occupancy << 18);
    diagonal_slider |= occupancy & (diagonal_slider << 36);
    return ((diagonal_slider << 9) & notFA);
}

U64 Bitboard::slide_NW(U64 diagonal_slider, U64 occupancy)const {
    occupancy &= notFH;
    diagonal_slider |= occupancy & (diagonal_slider << 7);
    occupancy &= (occupancy << 7);
    diagonal_slider |= occupancy & (diagonal_slider << 14);
    occupancy &= (occupancy << 14);
    diagonal_slider |= occupancy & (diagonal_slider << 28);
    return ((diagonal_slider << 7) & notFH);
}

U64 Bitboard::slide_SW(U64 diagonal_slider, U64 occupancy)const {
    occupancy &= notFH;
    diagonal_slider |= occupancy & (diagonal_slider >> 9);
    occupancy &= (occupancy >> 9);
    diagonal_slider |= occupancy & (diagonal_slider >> 18);
    occupancy &= (occupancy >> 18);
    diagonal_slider |= occupancy & (diagonal_slider >> 36);
    return ((diagonal_slider >> 9) & notFH);
}

U64 Bitboard::slide_SE(U64 diagonal_slider, U64 occupancy)const {
    occupancy &= notFA;
    diagonal_slider |= occupancy & (diagonal_slider >> 7);
    occupancy &= (occupancy >> 7);
    diagonal_slider |= occupancy & (diagonal_slider >> 14);
    occupancy &= (occupancy >> 14);
    diagonal_slider |= occupancy & (diagonal_slider >> 28);
    return ((diagonal_slider >> 7) & notFA);
}

U64 Bitboard::attacked(char attacker)const
{
    int index;
    U64 region = Xnone;
    U64 unoccupied = empty();
    U64 piece_set;

    //King Attacks
    piece_set = attacker == 'w' ? chessboard[WK] : chessboard[BK];
    region |= kingMask[getLSBIndex(piece_set)];

    //Cardinal Slide Attacks
    piece_set = attacker == 'w' ? chessboard[WQ] | chessboard[WR] : chessboard[BQ] | chessboard[BR];
    region |= slide_EAST(piece_set, unoccupied);
    region |= slide_NORTH(piece_set, unoccupied);
    region |= slide_WEST(piece_set, unoccupied);
    region |= slide_SOUTH(piece_set, unoccupied);

    //Diagonal Slide Attacks
    piece_set = attacker == 'w' ? chessboard[WQ] | chessboard[WB] : chessboard[BQ] | chessboard[BB];
    region |= slide_NE(piece_set, unoccupied);
    region |= slide_NW(piece_set, unoccupied);
    region |= slide_SW(piece_set, unoccupied);
    region |= slide_SE(piece_set, unoccupied);

    //Knight Attacks
    piece_set = attacker == 'w' ? chessboard[WN] : chessboard[BN];
    while (piece_set)
    {
        index = getLSBIndex(piece_set);
        piece_set &= piece_set - 1ULL;
        region |= knightMask[index];
    }

    //Pawn Attacks
    if (attacker == 'w')
    {
        region |= ((chessboard[WP] << 7) & ~fileH);
        region |= ((chessboard[WP] << 9) & ~fileA);
    }
    else
    {
        region |= ((chessboard[BP] >> 7) & ~fileA);
        region |= ((chessboard[BP] >> 9) & ~fileH);
    }
    return region;
}

bool Bitboard::inCheck()const
{
    U64 king = acting_color == 'w' ? chessboard[WK] : chessboard[BK];
    U64 attacked_region = acting_color == 'w' ? attacked('b') : attacked('w');
    return (bool)(king & attacked_region);
}

bool Bitboard::enChecked(Chessmove & action)
{
    piece_type t_src; //Type of piece moving
    piece_type t_dst; //Type of piece at destination (possibly none)
    U64 o_src; //Original state of moving piece's bitboard
    U64 o_dst; //Original state of destination piece's bitboard
    bool capture = false;

    //Computing acting piece + testing to see if capture occurred
    if (acting_color == 'w')
    {
        t_src = action.piece == 'P' ? piece_type::WP :
                action.piece == 'N' ? piece_type::WN :
                action.piece == 'B' ? piece_type::WB :
                action.piece == 'R' ? piece_type::WR :
                action.piece == 'Q' ? piece_type::WQ : piece_type::WK;
        o_src = chessboard[t_src];
        o_dst = bitMask[action.dst_i];
        if(o_dst & black())
        {
            capture = true;
            t_dst = (o_dst & chessboard[piece_type::BP]) ? piece_type::BP :
                    (o_dst & chessboard[piece_type::BN]) ? piece_type::BN :
                    (o_dst & chessboard[piece_type::BB]) ? piece_type::BB :
                    (o_dst & chessboard[piece_type::BR]) ? piece_type::BR :
                    (o_dst & chessboard[piece_type::BQ]) ? piece_type::BQ : piece_type::BK;
            o_dst = chessboard[t_dst];
        }
    }
    else
    {
        t_src = action.piece == 'p' ? piece_type::BP :
                action.piece == 'n' ? piece_type::BN :
                action.piece == 'b' ? piece_type::BB :
                action.piece == 'r' ? piece_type::BR :
                action.piece == 'q' ? piece_type::BQ : piece_type::BK;
        o_src = chessboard[t_src];
        o_dst = bitMask[action.dst_i];
        if(o_dst & white())
        {
            capture = true;
            t_dst = (o_dst & chessboard[piece_type::WP]) ? piece_type::WP :
                    (o_dst & chessboard[piece_type::WN]) ? piece_type::WN :
                    (o_dst & chessboard[piece_type::WB]) ? piece_type::WB :
                    (o_dst & chessboard[piece_type::WR]) ? piece_type::WR :
                    (o_dst & chessboard[piece_type::WQ]) ? piece_type::WQ : piece_type::WK;
            o_dst = chessboard[t_dst];
        }
    }

    //Perform move, evaluate check, then revert move
    chessboard[t_src] &= ~bitMask[action.src_i];
    chessboard[t_src] |= bitMask[action.dst_i];
    if (capture) chessboard[t_dst] &= ~bitMask[action.dst_i];
    bool result = inCheck();
    chessboard[t_src] = o_src;
    if (capture) chessboard[t_dst] = o_dst;
    if (capture) action.capture = true;
    return result;
}

bool Bitboard::isValidCastle(castle_type c_t, U64 attacked_region)const
{
    if(acting_color == 'w' && readPiece(AN_mapping::e1) == 'K')
    {
        if(c_t == castle_type::CWK && WK_castle && readPiece(AN_mapping::h1) == 'R')
        {
            return !(bool)((occupied() & CASTLE_OPEN_WK) | (attacked_region & CASTLE_NATK_WK));
        }
        else if(c_t == castle_type::CWQ && WQ_castle && readPiece(AN_mapping::a1) == 'R')
        {
            return !(bool)((occupied() & CASTLE_OPEN_WQ) | (attacked_region & CASTLE_NATK_WQ));
        }
    }
    else if(acting_color == 'b' && readPiece(AN_mapping::e8) == 'k')
    {
        if(c_t == castle_type::CBK && BK_castle && readPiece(AN_mapping::h8) == 'r')
        {
            return !(bool)((occupied() & CASTLE_OPEN_BK) | (attacked_region & CASTLE_NATK_BK));
        }
        else if(c_t == castle_type::CBQ && BQ_castle && readPiece(AN_mapping::a8) == 'r')
        {
            return !(bool)((occupied() & CASTLE_OPEN_BQ) | (attacked_region & CASTLE_NATK_BQ));
        }
    }
    return false;
}

Bitboard::Bitboard()
{
    chessboard[WK] = 0ULL;
    chessboard[WQ] = 0ULL;
    chessboard[WR] = 0ULL;
    chessboard[WB] = 0ULL;
    chessboard[WN] = 0ULL;
    chessboard[WP] = 0ULL;
    chessboard[BK] = 0ULL;
    chessboard[BQ] = 0ULL;
    chessboard[BR] = 0ULL;
    chessboard[BB] = 0ULL;
    chessboard[BN] = 0ULL;
    chessboard[BP] = 0ULL;
    acting_color = 'w';
    WK_castle = false;
    WQ_castle = false;
    BK_castle = false;
    BQ_castle = false;
    en_passant = -1;
    self_check = false;
    half_move_clock = 0;
    move_clock = 0;
    zobristKey = 0;
}

Bitboard::Bitboard(const Bitboard & src)
{
    chessboard[WK] = src.chessboard[WK];
    chessboard[WQ] = src.chessboard[WQ];
    chessboard[WR] = src.chessboard[WR];
    chessboard[WB] = src.chessboard[WB];
    chessboard[WN] = src.chessboard[WN];
    chessboard[WP] = src.chessboard[WP];
    chessboard[BK] = src.chessboard[BK];
    chessboard[BQ] = src.chessboard[BQ];
    chessboard[BR] = src.chessboard[BR];
    chessboard[BB] = src.chessboard[BB];
    chessboard[BN] = src.chessboard[BN];
    chessboard[BP] = src.chessboard[BP];
    acting_color = src.acting_color;
    WK_castle = src.WK_castle;
    WQ_castle = src.WQ_castle;
    BK_castle = src.BK_castle;
    BQ_castle = src.BQ_castle;
    en_passant = src.en_passant;
    self_check = src.self_check;
    half_move_clock = src.half_move_clock;
    move_clock = src.move_clock;
    zobristKey = src.zobristKey;
}

Bitboard & Bitboard::operator=(const Bitboard & rhs)
{
    chessboard[WK] = rhs.chessboard[WK];
    chessboard[WQ] = rhs.chessboard[WQ];
    chessboard[WR] = rhs.chessboard[WR];
    chessboard[WB] = rhs.chessboard[WB];
    chessboard[WN] = rhs.chessboard[WN];
    chessboard[WP] = rhs.chessboard[WP];
    chessboard[BK] = rhs.chessboard[BK];
    chessboard[BQ] = rhs.chessboard[BQ];
    chessboard[BR] = rhs.chessboard[BR];
    chessboard[BB] = rhs.chessboard[BB];
    chessboard[BN] = rhs.chessboard[BN];
    chessboard[BP] = rhs.chessboard[BP];
    acting_color = rhs.acting_color;
    WK_castle = rhs.WK_castle;
    WQ_castle = rhs.WQ_castle;
    BK_castle = rhs.BK_castle;
    BQ_castle = rhs.BQ_castle;
    en_passant = rhs.en_passant;
    self_check = rhs.self_check;
    half_move_clock = rhs.half_move_clock;
    move_clock = rhs.move_clock;
    zobristKey = rhs.zobristKey;
    return *this;
}

char Bitboard::readPiece(int index)const
{
    if(readBit(empty(), index)) return 'E'; //E represents empty space
    return (readBit(chessboard[WK], index) ? 'K' :
            readBit(chessboard[WQ], index) ? 'Q' :
            readBit(chessboard[WR], index) ? 'R' :
            readBit(chessboard[WB], index) ? 'B' :
            readBit(chessboard[WN], index) ? 'N' :
            readBit(chessboard[WP], index) ? 'P' :
            readBit(chessboard[BK], index) ? 'k' :
            readBit(chessboard[BQ], index) ? 'q' :
            readBit(chessboard[BR], index) ? 'r' :
            readBit(chessboard[BB], index) ? 'b' :
            readBit(chessboard[BN], index) ? 'n' :
            readBit(chessboard[BP], index) ? 'p' : '?');
}

void Bitboard::display()const
{
    char piece;
    cout << "    _ _ _ _ _ _ _ _ " << endl;
    cout << "  / -Current State- \\" << endl;
    for(int i = 7; i >= 0; i--)
    {
        cout << (i + 1) << " | ";

        for(int j = 0; j < 8; j++)
        {
            piece = readPiece((i*8)+j);
            if(piece == 'E') cout << "  ";
            else cout << piece << " ";
        }
        cout << "|   ";
        switch (i)
        {
            case 7:
                cout << " --- FLAGS --- " << endl;
                break;
            case 6:
                cout << "Zobrist ID: " << zobristKey << endl;
                break;
            case 5:
                cout << "Color: " << acting_color << endl;
                break;
            case 4:
                cout << "Enpassant Index: " << en_passant;
                if(en_passant > -1 && en_passant < 64)
                {
                    cout << " ( -> " << AN_string[en_passant] << ")";
                }
                cout << endl;
                break;
            case 3:
                cout << "WK/WQ/BK/BQ: " << WK_castle << WQ_castle << BK_castle << BQ_castle << endl;
                break;
            case 2:
                cout << "In Check: " << self_check << endl;
                break;
            case 1:
                cout << "Halfmoves: " << half_move_clock << endl;
                break;
            case 0:
                cout << "Relative Evaluation: " << evaluate(acting_color) << endl;
                break;
            default:
                break;
        }
    }
    cout << "  \\ _ _ _ _ _ _ _ _ /" << endl;
    cout << "    A B C D E F G H" << endl;

}

int Bitboard::countPieces() const
{
    return countBits(occupied());
}

int Bitboard::evaluate(char attacker)const
{
    int sums[5];
    int hc_mod = 0;
    if (attacker == 'w')
    {
        sums[0] = 9*countBits(chessboard[WQ]);
        sums[1] = 5*countBits(chessboard[WR]);
        sums[2] = 3*countBits(chessboard[WB] | chessboard[WN]);
        sums[3] = countBits(chessboard[WP]);
        sums[4] = 1000*countBits(chessboard[WK]);
        sums[0] -= 9*countBits(chessboard[BQ]);
        sums[1] -= 5*countBits(chessboard[BR]);
        sums[2] -= 3*countBits(chessboard[BB] | chessboard[BN]);
        sums[3] -= countBits(chessboard[BP]);
        sums[4] -= 1000*countBits(chessboard[BK]);
    }
    else
    {
        sums[0] = 9*countBits(chessboard[BQ]);
        sums[1] = 5*countBits(chessboard[BR]);
        sums[2] = 3*countBits(chessboard[BB] | chessboard[BN]);
        sums[3] = countBits(chessboard[BP]);
        sums[4] = 1000*countBits(chessboard[BK]);
        sums[0] -= 9*countBits(chessboard[WQ]);
        sums[1] -= 5*countBits(chessboard[WR]);
        sums[2] -= 3*countBits(chessboard[WB] | chessboard[WN]);
        sums[3] -= countBits(chessboard[WP]);
        sums[4] -= 1000*countBits(chessboard[WK]);
    }
    hc_mod = (half_move_clock < 20 ?  0 : half_move_clock*half_move_clock); //Encourage captures/pawn moves
    return (100*(sums[0] + sums[1] + sums[2] + sums[3] + sums[4])) - hc_mod;
}

GameStatus Bitboard::genMoves(vector<Chessmove> & validMoves)
{
    if (!validMoves.empty()) { cout << "WARNING: Generating moves into a non-empty vector..." << endl; }
    Chessmove evalMove; //Contains move being evaluated
    U64 not_ally;       //Stores !allied locations
    U64 enemies;        //Stores enemy pieces
    U64 piece_set;      //Stores pieces being evaluated
    U64 buffer;         //Stores intermediate processing results
    U64 attack_grid;    //Stores locations currently under attack
    U64 slide_root;     //Stores sliding piece during slide attacks
    int index;          //Stores current location being evaluated
    if (acting_color == 'w')
    {
        not_ally = ~white();
        enemies = black();
        attack_grid = attacked('b');
    }
    else
    {
        not_ally = ~black();
        enemies = white();
        attack_grid = attacked('w');
    }
    if (en_passant > -1) enemies |= bitMask[en_passant];
    U64 unoccupied = empty(); //Stores open spaces

    //Pawn Moves
    if (acting_color == 'w')
    {
        piece_set = chessboard[WP];
        evalMove.piece = 'P';
    }
    else
    {
        piece_set = chessboard[BP];
        evalMove.piece = 'p';
    }
    while (piece_set)
    {
        index = getLSBIndex(piece_set);
        piece_set &= piece_set - 1ULL;
        if (acting_color == 'w')
        {
            buffer = ((((bitMask[index] << 7) & ~fileH) | ((bitMask[index] << 9) & ~fileA)) & enemies)
                   | ((bitMask[index] << 8) & unoccupied);
            if(index > 7 && index < 16)
            {
                buffer |= (((bitMask[index] << 8) & unoccupied) << 8) & unoccupied;
            }
        }
        else
        {
            buffer = ((((bitMask[index] >> 7) & ~fileA) | ((bitMask[index] >> 9) & ~fileH)) & enemies)
                   | ((bitMask[index] >> 8) & unoccupied);
            if(index > 47 && index < 56)
            {
                buffer |= (((bitMask[index] >> 8) & unoccupied) >> 8) & unoccupied;
            }
        }
        evalMove.src_i = index;
        while (buffer)
        {
            index = getLSBIndex(buffer);
            buffer &= buffer - 1ULL;
            evalMove.dst_i = index;
            if(!enChecked(evalMove))
            {
                validMoves.push_back(evalMove);
                if (abs(evalMove.src_i - evalMove.dst_i) == 16) validMoves[validMoves.size() - 1].movtype = 'J';
                else if (index == en_passant) validMoves[validMoves.size() - 1].movtype = 'P';
                else if (index > 55 && acting_color == 'w')
                {
                    validMoves[validMoves.size() - 1].movtype = 'Q';
                    validMoves.push_back(evalMove);
                    validMoves[validMoves.size() - 1].movtype = 'N';
                    validMoves.push_back(evalMove);
                    validMoves[validMoves.size() - 1].movtype = 'R';
                    validMoves.push_back(evalMove);
                    validMoves[validMoves.size() - 1].movtype = 'B';
                }
                else if (index < 8 && acting_color == 'b')
                {
                    validMoves[validMoves.size() - 1].movtype = 'Q';
                    validMoves.push_back(evalMove);
                    validMoves[validMoves.size() - 1].movtype = 'N';
                    validMoves.push_back(evalMove);
                    validMoves[validMoves.size() - 1].movtype = 'R';
                    validMoves.push_back(evalMove);
                    validMoves[validMoves.size() - 1].movtype = 'B';
                }
                evalMove.capture = false;
            }

        }
    }

    //Knight Moves
    if (acting_color == 'w')
    {
        piece_set = chessboard[WN];
        evalMove.piece = 'N';
    }
    else
    {
        piece_set = chessboard[BN];
        evalMove.piece = 'n';
    }
    while (piece_set)
    {
        index = getLSBIndex(piece_set);
        piece_set &= piece_set - 1ULL;
        buffer = knightMask[index] & not_ally;
        evalMove.src_i = index;
        while (buffer)
        {
            index = getLSBIndex(buffer);
            buffer &= buffer - 1ULL;
            evalMove.dst_i = index;
            if(!enChecked(evalMove)) validMoves.push_back(evalMove); evalMove.capture = false;
        }
    }

    //Rook Moves
    if (acting_color == 'w')
    {
        piece_set = chessboard[WR];
        evalMove.piece = 'R';
    }
    else
    {
        piece_set = chessboard[BR];
        evalMove.piece = 'r';
    }
    while (piece_set)
    {
        index = getLSBIndex(piece_set);
        piece_set &= piece_set - 1ULL;
        buffer = Xnone;
        slide_root = bitMask[index];
        buffer |= slide_EAST(slide_root, unoccupied);
        buffer |= slide_NORTH(slide_root, unoccupied);
        buffer |= slide_WEST(slide_root, unoccupied);
        buffer |= slide_SOUTH(slide_root, unoccupied);
        buffer &= not_ally;
        evalMove.src_i = index;
        while (buffer)
        {
            index = getLSBIndex(buffer);
            buffer &= buffer - 1ULL;
            evalMove.dst_i = index;
            if(!enChecked(evalMove)) validMoves.push_back(evalMove); evalMove.capture = false;
        }
    }

    //Queen Moves
    if (acting_color == 'w')
    {
        piece_set = chessboard[WB];
        evalMove.piece = 'B';
    }
    else
    {
        piece_set = chessboard[BB];
        evalMove.piece = 'b';
    }
    while (piece_set)
    {
        index = getLSBIndex(piece_set);
        piece_set &= piece_set - 1ULL;
        buffer = Xnone;
        slide_root = bitMask[index];
        buffer |= slide_NE(slide_root, unoccupied);
        buffer |= slide_NW(slide_root, unoccupied);
        buffer |= slide_SW(slide_root, unoccupied);
        buffer |= slide_SE(slide_root, unoccupied);
        buffer &= not_ally;
        evalMove.src_i = index;
        while (buffer)
        {
            index = getLSBIndex(buffer);
            buffer &= buffer - 1ULL;
            evalMove.dst_i = index;
            if(!enChecked(evalMove)) validMoves.push_back(evalMove); evalMove.capture = false;
        }
    }

    //Queen Moves
    if (acting_color == 'w')
    {
        piece_set = chessboard[WQ];
        evalMove.piece = 'Q';
    }
    else
    {
        piece_set = chessboard[BQ];
        evalMove.piece = 'q';
    }
    while (piece_set)
    {
        index = getLSBIndex(piece_set);
        piece_set &= piece_set - 1ULL;
        buffer = Xnone;
        slide_root = bitMask[index];
        buffer |= slide_EAST(slide_root, unoccupied);
        buffer |= slide_NORTH(slide_root, unoccupied);
        buffer |= slide_WEST(slide_root, unoccupied);
        buffer |= slide_SOUTH(slide_root, unoccupied);
        buffer |= slide_NE(slide_root, unoccupied);
        buffer |= slide_NW(slide_root, unoccupied);
        buffer |= slide_SW(slide_root, unoccupied);
        buffer |= slide_SE(slide_root, unoccupied);
        buffer &= not_ally;
        evalMove.src_i = index;
        while (buffer)
        {
            index = getLSBIndex(buffer);
            buffer &= buffer - 1ULL;
            evalMove.dst_i = index;
            if(!enChecked(evalMove)) validMoves.push_back(evalMove); evalMove.capture = false;
        }
    }

    //King Moves
    if (acting_color == 'w')
    {
        piece_set = chessboard[WK];
        evalMove.piece = 'K';
    }
    else
    {
        piece_set = chessboard[BK];
        evalMove.piece = 'k';
    }
    index = getLSBIndex(piece_set);
    buffer = kingMask[index] & not_ally & ~attack_grid; //Can't move into check.
    evalMove.src_i = index;
    while (buffer)
    {
        index = getLSBIndex(buffer);
        buffer &= buffer - 1ULL;
        evalMove.dst_i = index;
        if(!enChecked(evalMove)) validMoves.push_back(evalMove); evalMove.capture = false;
    }
    evalMove.movtype = 'C'; //Begin Checking for Castling Moves
    if (acting_color == 'w')
    {
        if (WK_castle && isValidCastle(castle_type::CWK, attack_grid))
        {
            evalMove.dst_i = AN_mapping::g1;
            validMoves.push_back(evalMove);
        }
        if (WQ_castle && isValidCastle(castle_type::CWQ, attack_grid))
        {
            evalMove.dst_i = AN_mapping::c1;
            validMoves.push_back(evalMove);
        }
    }
    else
    {
        if (BK_castle && isValidCastle(castle_type::CBK, attack_grid))
        {
            evalMove.dst_i = AN_mapping::g8;
            validMoves.push_back(evalMove);
        }
        if (BQ_castle && isValidCastle(castle_type::CBQ, attack_grid))
        {
            evalMove.dst_i = AN_mapping::c8;
            validMoves.push_back(evalMove);
        }
    }

    //Game Status
    if (validMoves.empty()) //If no moves were generated
    {
        if (self_check) return GameStatus::Checkmate; //Checkmate
        else return GameStatus::Stalemate; //Stalemate
    }
    else return GameStatus::Ongoing; //Game Continues
}

void Bitboard::performAction(const Chessmove & action)
{
    piece_type t_src;
    piece_type t_dst;
    U64 captureTarget;
    bool capture = false;
    if (acting_color == 'w')
    {
        t_src = action.piece == 'P' ? piece_type::WP :
                action.piece == 'N' ? piece_type::WN :
                action.piece == 'B' ? piece_type::WB :
                action.piece == 'R' ? piece_type::WR :
                action.piece == 'Q' ? piece_type::WQ : piece_type::WK;
        captureTarget = bitMask[action.dst_i];
        if(captureTarget & black())
        {
            capture = true;
            t_dst = (captureTarget & chessboard[piece_type::BP]) ? piece_type::BP :
                    (captureTarget & chessboard[piece_type::BN]) ? piece_type::BN :
                    (captureTarget & chessboard[piece_type::BB]) ? piece_type::BB :
                    (captureTarget & chessboard[piece_type::BR]) ? piece_type::BR :
                    (captureTarget & chessboard[piece_type::BQ]) ? piece_type::BQ : piece_type::BK;
        }
    }
    else
    {
        t_src = action.piece == 'p' ? piece_type::BP :
                action.piece == 'n' ? piece_type::BN :
                action.piece == 'b' ? piece_type::BB :
                action.piece == 'r' ? piece_type::BR :
                action.piece == 'q' ? piece_type::BQ : piece_type::BK;
        captureTarget = bitMask[action.dst_i];
        if(captureTarget & white())
        {
            capture = true;
            t_dst = (captureTarget & chessboard[piece_type::WP]) ? piece_type::WP :
                    (captureTarget & chessboard[piece_type::WN]) ? piece_type::WN :
                    (captureTarget & chessboard[piece_type::WB]) ? piece_type::WB :
                    (captureTarget & chessboard[piece_type::WR]) ? piece_type::WR :
                    (captureTarget & chessboard[piece_type::WQ]) ? piece_type::WQ : piece_type::WK;
        }
    }
    chessboard[t_src] &= ~bitMask[action.src_i];
    chessboard[t_src] |= bitMask[action.dst_i];
    if (capture) chessboard[t_dst] &= ~bitMask[action.dst_i];

    //Flag Updating
    if (WK_castle)
    {
        if (action.piece == 'K') WK_castle = false;
        else if (action.piece == 'R' && action.src_i == AN_mapping::h1)
        {
            WK_castle = false;
        }
    }
    if (WQ_castle)
    {
        if (action.piece == 'K') WQ_castle = false;
        else if (action.piece == 'R' && action.src_i == AN_mapping::a1)
        {
            WQ_castle = false;
        }
    }
    if (BK_castle)
    {
        if (action.piece == 'k') BK_castle = false;
        else if (action.piece == 'r' && action.src_i == AN_mapping::h8)
        {
            BK_castle = false;
        }
    }
    if (BQ_castle)
    {
        if (action.piece == 'k') BQ_castle = false;
        else if (action.piece == 'r' && action.src_i == AN_mapping::a8)
        {
            BQ_castle = false;
        }
    }

    //Special Move Handling
    if (action.movtype != 'S')
    {
        if (action.movtype == 'J')
        {
            if (acting_color == 'w') en_passant = action.src_i + 8;
            else en_passant = action.src_i - 8;
        }
        else if (action.movtype == 'P')
        {
            if (acting_color == 'w') chessboard[piece_type::BP] &= ~(bitMask[action.dst_i] >> 8);
            else chessboard[piece_type::WP] &= ~(bitMask[action.dst_i] << 8);
        }
        else if (action.movtype == 'C')
        {
            if (acting_color == 'w')
            {
                if (action.dst_i == AN_mapping::g1)
                {
                    chessboard[piece_type::WR] &= ~(bitMask[AN_mapping::h1]);
                    chessboard[piece_type::WR] |= bitMask[AN_mapping::f1];
                }
                else if (action.dst_i == AN_mapping::c1)
                {
                    chessboard[piece_type::WR] &= ~(bitMask[AN_mapping::a1]);
                    chessboard[piece_type::WR] |= bitMask[AN_mapping::d1];
                }
            }
            else
            {
                if (action.dst_i == AN_mapping::g8)
                {
                    chessboard[piece_type::BR] &= ~(bitMask[AN_mapping::h8]);
                    chessboard[piece_type::BR] |= bitMask[AN_mapping::f8];
                }
                else if (action.dst_i == AN_mapping::c8)
                {
                    chessboard[piece_type::BR] &= ~(bitMask[AN_mapping::a8]);
                    chessboard[piece_type::BR] |= bitMask[AN_mapping::d8];
                }
            }
        }
        else if (action.movtype == 'Q')
        {
            chessboard[t_src] &= ~bitMask[action.dst_i];
            if (acting_color == 'w') chessboard[piece_type::WQ] |= bitMask[action.dst_i];
            else chessboard[piece_type::BQ] |= bitMask[action.dst_i];
        }
        else if (action.movtype == 'N')
        {
            chessboard[t_src] &= ~bitMask[action.dst_i];
            if (acting_color == 'w') chessboard[piece_type::WN] |= bitMask[action.dst_i];
            else chessboard[piece_type::BN] |= bitMask[action.dst_i];
        }
        else if (action.movtype == 'R')
        {
            chessboard[t_src] &= ~bitMask[action.dst_i];
            if (acting_color == 'w') chessboard[piece_type::WR] |= bitMask[action.dst_i];
            else chessboard[piece_type::BR] |= bitMask[action.dst_i];
        }
        else if (action.movtype == 'B')
        {
            chessboard[t_src] &= ~bitMask[action.dst_i];
            if (acting_color == 'w') chessboard[piece_type::WB] |= bitMask[action.dst_i];
            else chessboard[piece_type::BB] |= bitMask[action.dst_i];
        }
    }
    if (action.movtype != 'J') en_passant = -1;

    acting_color = (acting_color == 'w' ? 'b' : 'w'); //Swap turns
    self_check = inCheck();
    if (capture || action.piece == 'p' || action.piece == 'P') half_move_clock = 0; else half_move_clock++;
    if (acting_color == 'w') move_clock++;
    zobristKey = Lookup::ZobristHash(chessboard);
}

void Bitboard::loadFEN(const string & fen)
{
    int iterations = 0;
    char storage = '/';
    en_passant = -1;
    for (int counter = 0; counter < 64; counter++)
    {
        if(storage < '0' || storage > '8') //If empty space, skip over board slot
        {
            storage = fen[iterations];
            iterations++;
        }
        while(storage == '/' || storage == '0') //Find next character
        {
            storage = fen[iterations];
            iterations++;
        }
        chessboard[WK] = chessboard[WK] << 1;
        chessboard[WQ] = chessboard[WQ] << 1;
        chessboard[WR] = chessboard[WR] << 1;
        chessboard[WB] = chessboard[WB] << 1;
        chessboard[WN] = chessboard[WN] << 1;
        chessboard[WP] = chessboard[WP] << 1;
        chessboard[BK] = chessboard[BK] << 1;
        chessboard[BQ] = chessboard[BQ] << 1;
        chessboard[BR] = chessboard[BR] << 1;
        chessboard[BB] = chessboard[BB] << 1;
        chessboard[BN] = chessboard[BN] << 1;
        chessboard[BP] = chessboard[BP] << 1;
        if(storage > '0' && storage < '9') //If empty space, continue skipping board slots.
        {
            storage--;

        }
        else //Else, determine what piece is to be placed.
        {
            switch (storage) {
                case 'K':
                    chessboard[WK] += 1ULL;
                    break;
                case 'Q':
                    chessboard[WQ] += 1ULL;
                    break;
                case 'R':
                    chessboard[WR] += 1ULL;
                    break;
                case 'B':
                    chessboard[WB] += 1ULL;
                    break;
                case 'N':
                    chessboard[WN] += 1ULL;
                    break;
                case 'P':
                    chessboard[WP] += 1ULL;
                    break;
                case 'k':
                    chessboard[BK] += 1ULL;
                    break;
                case 'q':
                    chessboard[BQ] += 1ULL;
                    break;
                case 'r':
                    chessboard[BR] += 1ULL;
                    break;
                case 'b':
                    chessboard[BB] += 1ULL;
                    break;
                case 'n':
                    chessboard[BN] += 1ULL;
                    break;
                case 'p':
                    chessboard[BP] += 1ULL;
                    break;
                default:
                    break;
            }
        }
    }
    for (U64 &i : chessboard)
    {
        i = mirrorBitboard(i);
    }
    storage = fen[iterations];
    iterations++;

    while(storage != 'w' && storage != 'b') //Get player color
    {
        storage = fen[iterations];
        iterations++;
    }
    acting_color = storage;
    while(storage != 'K' && storage != 'Q' && storage != 'k' && storage != 'q' && storage != '-') //Castling Flags
    {
        storage = fen[iterations];
        iterations++;
    }
    if(storage == 'K')
    {
        WK_castle = true;
        storage = fen[iterations];
        iterations++;
    }
    else WK_castle = false;
    if(storage == 'Q')
    {
        WQ_castle = true;
        storage = fen[iterations];
        iterations++;
    }
    else WQ_castle = false;
    if(storage == 'k')
    {
        BK_castle = true;
        storage = fen[iterations];
        iterations++;
    }
    else BK_castle = false;
    BQ_castle = (storage == 'q');
    storage = fen[iterations];
    iterations++;
    while(storage == ' ')
    {
        storage = fen[iterations];
        iterations++;
    }
    if(storage != '-') //Enpassant Flags
    {
        en_passant = (int)(storage - 'a');
        storage = fen[iterations];
        iterations++;
        en_passant += 8 * ((int)(storage - '0') - 1);
    }
    storage = fen[iterations];
    iterations++;
    while(storage == ' ')
    {
        storage = fen[iterations];
        iterations++;
    }
    string half_count = "";
    while(storage != ' ')
    {
        half_count.push_back(storage);
        storage = fen[iterations];
        iterations++;
    }
    half_count.push_back('\0');
    half_move_clock = stoi(half_count);
    while(storage == ' ')
    {
        storage = fen[iterations];
        iterations++;
    }
    string full_count = "0";
    while(iterations <= fen.size())
    {
        full_count.push_back(storage);
        storage = fen[iterations];
        iterations++;
    }
    full_count.push_back('\0');
    move_clock = stoi(full_count);
    self_check = inCheck();
    zobristKey = Lookup::ZobristHash(chessboard);
}