///////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMMER: DANIEL FUCHS
/// CLASS/SECT: CS5400A - ARTIFICIAL INTELLIGENCE
/// ASSIGNMENT: CHESS GAME ASSIGNMENT: PART #4
/// DATE: 4/12/18
/// DESC: Class implementation file for Chessmove.
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "lookup.h"
#include "chessmove.h"
using namespace std;

//Parameterized Constructor
Chessmove::Chessmove(const int source, const char moving_piece, const int destination)
{
    src_i = source;
    dst_i = destination;
    piece = moving_piece;
    movtype = 'S';
    capture = false;
}

bool Chessmove::operator==(const Chessmove & rhs)const
{
    return ((src_i == rhs.src_i) && (dst_i == rhs.dst_i));
}

ostream & operator<<(ostream & os, const Chessmove & rhs)
{
    cout << (char)(getFile[rhs.src_i]+'a') << getRank[rhs.src_i]+1 << " to ";
    cout << (char)(getFile[rhs.dst_i]+'a') << getRank[rhs.dst_i]+1 << ": ";
    cout << rhs.piece << " T[" << rhs.movtype << (rhs.capture ? "] (X)" : "]") << endl;
    return os;
}


