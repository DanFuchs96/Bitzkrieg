///////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMMER: DANIEL FUCHS
/// CLASS/SECT: CS5400A - ARTIFICIAL INTELLIGENCE
/// ASSIGNMENT: CHESS GAME ASSIGNMENT: PART #4
/// DATE: 4/12/18
/// DESC: Class implementation file for class "ChessNode".
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <deque>
#include "bitboard.h"
#include "lookup.h"
#include "chessmove.h"
#include "chessnode.h"
using namespace std;

Chessmove ChessNode::pastMoves[8];
char ChessNode::operating_player;

void ChessNode::setHistory(const Chessmove past[])
{
    for (int i = 0; i < 8; i++) history[i] = past[i];
}

void ChessNode::updateHistory(const Chessmove & action, const Chessmove old[])
{
    history[0] = action;
    for (int i = 1; i < 8; i++) history[i] = old[i-1];
}

void ChessNode::setCondition()
{
    GameStatus status = state.genMoves(actions);
    if (status == GameStatus::Checkmate)
    {
        if (state.acting_color == operating_player)
        {
            condition = State_Condition::Loss;
        }
        else condition = State_Condition::Victory;
    }
    else if (status == GameStatus::Stalemate)
    {
        condition = State_Condition::Draw_Stale;
    }
    else if (status == GameStatus::Ongoing)
    {
        condition = State_Condition::Nonterminal;

        if (!(state.chessboard[WQ] | state.chessboard[BQ] | state.chessboard[WR] |
              state.chessboard[BR] | state.chessboard[WP] | state.chessboard[BP] ) ) //Sufficient Material Check
        {
            bool knights = state.chessboard[WN] || state.chessboard[BN];
            bool bishops = state.chessboard[WB] || state.chessboard[BB];
            if (knights)
            {
                if(!state.chessboard[WN] && !bishops)
                {
                    condition = State_Condition::Draw_IM;
                }
                else if(!state.chessboard[BN] && !bishops)
                {
                    condition = State_Condition::Draw_IM;
                }
            }
            else if (bishops)
            {
                bool dif_wb = (state.chessboard[WB] & light) && (state.chessboard[WB] & dark);
                bool dif_bb = (state.chessboard[BB] & light) && (state.chessboard[BB] & dark);
                if(!(dif_bb || dif_wb))
                {
                    condition = State_Condition::Draw_IM;
                }
            }
            else condition = State_Condition::Draw_IM;
        }

        if (state.half_move_clock > 99) //Half-move Check
        {
            condition = State_Condition::Draw_50;
        }
        else if (history[0] == history[4]) //Repetition Check
        {
            if (history[1] == history[5] && history[2] == history[6] && history[3] == history[7])
            {
                bool not_repeated = false;
                for (int i = 0; i < 8 && !not_repeated; i++) //Check for irreversible moves
                {
                    if (history[i].src_i == -1  || history[i].piece == 'p' ||
                        history[i].piece == 'P' || history[i].capture)
                    {
                        not_repeated = true; //Irreversible move found -> not repeated
                    }
                }
                if (!not_repeated) condition = State_Condition::Draw_Rep;
            }
        }
    }
}

void ChessNode::evaluateState()
{
    heuristic_value = state.evaluate(operating_player);    //Perform state evaluation
    if(operating_player == 'w') heuristic_value += 100*state.countBits(state.chessboard[WQ]); //Queen reward
    else                        heuristic_value += 100*state.countBits(state.chessboard[BQ]); //Queen reward
    if (condition != State_Condition::Nonterminal) //If terminal state, compute utility instead
    {
        switch (condition)
        {
            case State_Condition::Loss:
                heuristic_value = UTILITY_MIN;
                break;
            case State_Condition::Victory:
                heuristic_value = UTILITY_MAX;
                break;
            case State_Condition::Draw_Stale:
            case State_Condition::Draw_50:
            case State_Condition::Draw_Rep:
            case State_Condition::Draw_IM:
                heuristic_value = UTILITY_DRAW;
                break;
            default:
                break;
        }
        heuristic_value -= 32*state.numTurn(); //Encourage shorter games
    }
    else
    {
        int repetition_penalty = 0;
        if (history[0].src_i != -1 && history[0] == history[4]) repetition_penalty++;
        if (history[1].src_i != -1 && history[1] == history[5]) repetition_penalty++;
        if (history[2].src_i != -1 && history[2] == history[6]) repetition_penalty++;
        if (history[3].src_i != -1 && history[3] == history[7]) repetition_penalty++;
        heuristic_value -= (repetition_penalty*repetition_penalty*repetition_penalty*16);
    }
}

ChessNode::ChessNode(const Bitboard & board, const vector<Chessmove> & moves)
{
    //Root Initializations
    operating_player = board.acting_color;
    for (int i = 0; i < 8; i++)
    {
        ChessNode::pastMoves[i] = moves[i];
    }

    //Local Initializations
    state = board;
    quiescence = 0;
    setHistory(ChessNode::pastMoves);
    setCondition();
    evaluateState();
}

ChessNode::ChessNode(const ChessNode & parent, const Chessmove & action)
{
    state = parent.state;
    state.performAction(action);
    plys = parent.plys + 1;
    quiescence = abs(parent.state.evaluate(operating_player) - state.evaluate(operating_player));
    updateHistory(action, parent.history);
    setCondition();
    evaluateState();
}

void ChessNode::display()const
{
    cout << endl << operating_player << ": C" << condition << "=>Q" << quiescence
         << "+(" << heuristic_value << ")[" << state.numTurn() << "]" << endl;
    state.display();
}