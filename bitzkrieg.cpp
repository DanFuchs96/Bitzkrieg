///////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMMER: DANIEL FUCHS
/// CLASS/SECT: CS5400A - ARTIFICIAL INTELLIGENCE
/// ASSIGNMENT: CHESS GAME ASSIGNMENT: PART #4
/// DATE: 4/12/18
/// DESC: Class implementation file for class "Bitzkrieg".
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <ctime>
#include "bitboard.h"
#include "lookup.h"
#include "chessmove.h"
#include "bitzkrieg.h"
using namespace std;

int Bitzkrieg::TLHTQSABDLMM_Max(ChessNode* ref_node, int depth, int qs_d, int alpha, int beta)
{
    if (Lookup::TimeslotOver() || ref_node->status() != State_Condition::Nonterminal || (depth < 1 && qs_d < 1))
    {
        return ref_node->heuristic();
    }
    else
    {
        if (depth < 1) //If maximum depth was reached
        {
            if (ref_node->isNotQuiescent()) //If state is non-quiescent, extend search
            {
                depth++;
                qs_d--;
            }
            else return ref_node->heuristic(); //If state is quiescent, end search
        }

        const vector<Chessmove> & validMoves = ref_node->ACTIONS();
        int num_moves = (int)validMoves.size();
        int maximum = INT_MIN;
        int value;
        int index = -1;
        U64 transposition_key;

        vector<ChessNode*> branch;         //Stores all children
        pair<int, int> temp;               //Associates node with an index
        vector< pair<int, int> > frontier; //Stores child indexes sorted by history table values
        for (int i = 0; i < num_moves; i++)
        {
            branch.push_back(RESULTS(ref_node, validMoves[i]));
            temp = { HistoryTable[branch[i]->zobristKey()], i };
            frontier.push_back(temp);
        }
        Lookup::ArrangeNodes(frontier);
        for (int i = 0; i < num_moves; i++)
        {
            transposition_key = branch[frontier[i].second]->zobristKey();
            if(Lookup::InTranspositionTable(transposition_key, ref_node->numPlys()))
            {
                value = TranspositionTable[transposition_key].value;
                alpha = TranspositionTable[transposition_key].alpha;
                beta = TranspositionTable[transposition_key].beta;
            }
            else
            {
                value = TLHTQSABDLMM_Min(branch[frontier[i].second], depth - 1, qs_d, alpha, beta);
                Lookup::UpdateTranspositionTable(transposition_key);
                TranspositionTable[transposition_key].value = value;
                TranspositionTable[transposition_key].alpha = alpha;
                TranspositionTable[transposition_key].beta = beta;
                TranspositionTable[transposition_key].plys = ref_node->numPlys();
            }
            if (value > maximum) { maximum = value; index = frontier[i].second; }
            if (value >= beta)
            {
                Lookup::UpdateHistoryTable(branch[index]->zobristKey());
                for (int j = 0; j < num_moves; j++) delete branch[j];
                return value;
            }
            if (value > alpha)
            {
                alpha = value;
                TranspositionTable[transposition_key].alpha = alpha;
            }
        }
        Lookup::UpdateHistoryTable(branch[index]->zobristKey());
        for (int i = 0; i < num_moves; i++) delete branch[i];
        return maximum;
    }
}

int Bitzkrieg::TLHTQSABDLMM_Min(ChessNode* ref_node, int depth, int qs_d, int alpha, int beta)
{
    if (Lookup::TimeslotOver() || ref_node->status() != State_Condition::Nonterminal || (depth < 1 && qs_d < 1))
    {
        return ref_node->heuristic();
    }
    else
    {
        if (depth < 1) //If maximum depth was reached
        {
            if (ref_node->isNotQuiescent()) //If state is non-quiescent, extend search
            {
                depth++;
                qs_d--;
            }
            else return ref_node->heuristic(); //If state is quiescent, end search
        }

        const vector<Chessmove> & validMoves = ref_node->ACTIONS();
        int num_moves = (int)validMoves.size();
        int minimum = INT_MAX;
        int value;
        int index = -1;
        U64 transposition_key;

        vector<ChessNode*> branch;         //Stores all children
        pair<int, int> temp;               //Associates node with an index
        vector< pair<int, int> > frontier; //Stores child indexes sorted by history table values
        for (int i = 0; i < num_moves; i++)
        {
            branch.push_back(RESULTS(ref_node, validMoves[i]));
            temp = { HistoryTable[branch[i]->zobristKey()], i };
            frontier.push_back(temp);
        }
        Lookup::ArrangeNodes(frontier);
        for (int i = 0; i < num_moves; i++)
        {
            transposition_key = branch[frontier[i].second]->zobristKey();
            if(Lookup::InTranspositionTable(transposition_key, ref_node->numPlys()))
            {
                value = TranspositionTable[transposition_key].value;
                alpha = TranspositionTable[transposition_key].alpha;
                beta = TranspositionTable[transposition_key].beta;
            }
            else
            {
                value = TLHTQSABDLMM_Max(branch[frontier[i].second], depth - 1, qs_d, alpha, beta);
                Lookup::UpdateTranspositionTable(transposition_key);
                TranspositionTable[transposition_key].value = value;
                TranspositionTable[transposition_key].alpha = alpha;
                TranspositionTable[transposition_key].beta = beta;
                TranspositionTable[transposition_key].plys = ref_node->numPlys();
            }
            if (value < minimum) { minimum = value; index = frontier[i].second; }
            if (value <= alpha)
            {
                Lookup::UpdateHistoryTable(branch[index]->zobristKey());
                for (int j = 0; j < num_moves; j++) delete branch[j];
                return value;
            }
            if (value < beta)
            {
                beta = value;
                TranspositionTable[transposition_key].beta = beta;
            }
        }
        Lookup::UpdateHistoryTable(branch[index]->zobristKey());
        for (int i = 0; i < num_moves; i++) delete branch[i];
        return minimum;
    }
}

Chessmove Bitzkrieg::TLHTQSABDLMM_Recursive(ChessNode* root_node, const int depth, const int qs_d)
{
    int alpha = INT_MIN; //Alpha Value
    int beta = INT_MAX;  //Beta Value

    const vector<Chessmove> & validMoves = root_node->ACTIONS();
    int num_moves = (int)validMoves.size();
    int maximum = INT_MIN;
    int value = -1;
    int index = 0;

    if (root_node->status() != State_Condition::Nonterminal)
    {
        cerr << "ERROR: Game is already complete." << endl;
        throw Chessmove(-1);
    }
    else if (num_moves == 0)
    {
        cerr << "ERROR: No moves available in nonterminal state." << endl;
        throw Chessmove(-1);
    }

    vector<ChessNode*> branch;         //Stores all children
    pair<int, int> temp;               //Associates node with an index
    vector< pair<int, int> > frontier; //Stores child indexes sorted by history table values
    for (int i = 0; i < num_moves; i++)
    {
        branch.push_back(RESULTS(root_node, validMoves[i]));
        temp = { HistoryTable[branch[i]->zobristKey()], i };
        frontier.push_back(temp);
    }
    Lookup::ArrangeNodes(frontier);
    for (int i = 0; i < num_moves; i++)
    {
        value = TLHTQSABDLMM_Min(branch[frontier[i].second], depth - 1, qs_d, alpha, beta);
        if (value > maximum) { maximum = value; index = frontier[i].second; }
        if (value > alpha) alpha = value;
    }
    Lookup::UpdateHistoryTable(branch[index]->zobristKey());
    for (int i = 0; i < num_moves; i++) delete branch[i];
    return validMoves[index];
}

Bitzkrieg::Bitzkrieg()
{
    current_state.loadFEN();
    Chessmove null_move(-1);
    for (int i = 0; i < 8; i++)
    {
        pastMoves.push_back(null_move);
    }
    root_node = new ChessNode(current_state, pastMoves);
}

Bitzkrieg::Bitzkrieg(const string & fen, const vector<Chessmove> & moves)
{
    current_state.loadFEN(fen);
    player = current_state.getColor();
    Chessmove null_move(-1);
    for (int i = 0; i < 8; i++)
    {
        pastMoves.push_back(null_move);
    }
    root_node = new ChessNode(current_state, pastMoves);
}

void Bitzkrieg::execute(const Chessmove & action)
{
    current_state.performAction(action);
    for (int i = 1; i < 8; i++) //Update History
    {
        pastMoves[i - 1] = pastMoves[i];
    }
    pastMoves[0] = action;
    delete root_node;
    root_node = new ChessNode(current_state, pastMoves);
}

void Bitzkrieg::display()
{
    root_node->display();
}

vector<Chessmove> Bitzkrieg::ACTIONS()
{
    return root_node->ACTIONS();
}

ChessNode* Bitzkrieg::RESULTS(ChessNode* parent, const Chessmove & action)
{
    return new ChessNode(*parent, action);
}

int Bitzkrieg::countPieces() const
{
    return current_state.countPieces();
}

void Bitzkrieg::printMoves(const vector<Chessmove> & move)
{
    string src, dst, piece;
    for (int i = 0; i < (int)move.size(); i++)
    {
        src = AN_string[move[i].src_i];
        dst = AN_string[move[i].dst_i];
        switch (move[i].piece)
        {
            case 'K':
                piece = "White King";
                break;
            case 'Q':
                piece = "White Queen";
                break;
            case 'R':
                piece = "White Rook";
                break;
            case 'B':
                piece = "White Bishop";
                break;
            case 'N':
                piece = "White Knight";
                break;
            case 'P':
                piece = "White Pawn";
                break;
            case 'k':
                piece = "Black King";
                break;
            case 'q':
                piece = "Black Queen";
                break;
            case 'r':
                piece = "Black Rook";
                break;
            case 'b':
                piece = "Black Bishop";
                break;
            case 'n':
                piece = "Black Knight";
                break;
            case 'p':
                piece = "Black Pawn";
                break;
            default:
                piece = "MissingNam.";
                break;
        }
        if(move[i].movtype == 'S' || move[i].movtype == 'J' || move[i].movtype == 'P')
        {
            cout << piece << " from " << src << " to " << dst << "." << endl;
        }
        else if(move[i].movtype == 'C')
        {
            cout << piece << " castles to " << dst << "." << endl;
        }
        else if(move[i].movtype == 'Q' || move[i].movtype == 'R' || move[i].movtype == 'B' || move[i].movtype == 'N')
        {
            cout << piece << " from " << src << " to " << dst << "; ";
            cout << "Promotes to ";
            cout << (move[i].movtype == 'Q' ? "Queen" :
                     move[i].movtype == 'R' ? "Rook" :
                     move[i].movtype == 'B' ? "Bishop" : "Knight")
                 << "." << endl;
        }
    }
}

Chessmove Bitzkrieg::TLHTQSID_ABDLMM_Algorithm(const int max_depth, const int qs_depth)
{
    if (max_depth < 1) { cout << "WARNING: Bad Depth Call" << endl; return Chessmove(); };
    int level = 0;
    Chessmove processing_move;
    Chessmove best_move;

    //Time allowed per turn is managed by the Lookup Class.
    if(root_node->status() == State_Condition::Nonterminal)     //If the game has not ended
    {
        while (++level <= max_depth && !Lookup::TimeslotOver()) //If time/depth limit not reached, iterate
        {
            processing_move = TLHTQSABDLMM_Recursive(root_node, level, qs_depth); //Generate move for specified depth
            if(!Lookup::TimeslotOver() || level < 2) //If time expired, ignore incoming "rushed" move
            {
                best_move = processing_move;
            }
            Lookup::FlushTranspositionTable();
        }
        Lookup::FlushHistoryTable();
        return best_move;
    }
    else //Game is already over
    {
        string player = (current_state.getColor() == 'w' ? "Player White " : "Player Black ");
        string enemy = (current_state.getColor() == 'b' ? "Player White " : "Player Black ");
        switch (root_node->status()) //End state handling
        {
            case State_Condition::Victory:
                cout << player << "has checkmated " << enemy << "!" << endl;
                break;
            case State_Condition::Loss:
                cout << enemy << "has checkmated " << player << "!" << endl;
                break;
            case State_Condition::Draw_Stale:
                cout << "Draw: A stalemate has occurred." << endl;
                break;
            case State_Condition::Draw_50:
                cout << "Draw: The fifty move rule has triggered." << endl;
                break;
            case State_Condition::Draw_Rep:
                cout << "Draw: Threefold repetition has occurred." << endl;
                break;
            case State_Condition::Draw_IM:
                cout << "Draw: Insufficient material remains." << endl;
                break;
            default:
                break;
        }
        return Chessmove(-1);
    }
}