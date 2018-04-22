///////////////////////////////////////////////////////////////////////////////////////////////////
/// PROGRAMMER: DANIEL FUCHS
/// CLASS/SECT: CS5400A - ARTIFICIAL INTELLIGENCE
/// ASSIGNMENT: CHESS GAME ASSIGNMENT: PART #4
/// DATE: 4/22/18
/// DESC: Simple file to cause the AI to play against itself.
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include "bitboard.h"
#include "lookup.h"
#include "chessmove.h"
#include "chessnode.h"
#include "bitzkrieg.h"
using namespace std;

/// This is a very simple program to play the AI versus itself. In a more practical implementation, there
/// should be two instances created, and after making a move they should be given a FEN string of the resulting
/// state rather than making use of their execution function. However, execute can be used so that an instance
/// plays an entire game within itself. The Lookup Class can be used to schedule time/depth settings for both
/// via Lookup:UPDATE(remaining_time, remaining_piece_count) calls, after an initial Lookup::INIT() call.

int main()
{
    // INITIALIZATION
    srand(time(NULL));  //Seeds RNG
    clock_t t_time;     //Stores runtime
    Lookup::INIT();     //Initialize Lookup Values/Table
    Chessmove move(-2); //Stores move to be made
    vector<Chessmove> nullmoves; for (int i = 0; i < 8; i++) nullmoves.emplace_back(Chessmove(-1));

    Bitzkrieg Instance("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0", nullmoves);
    //Note that instance generally should be loaded with the default fen string and an "empty" move history.


    // SETTINGS
    double time = 300*1000000;         //Time of test game (in microseconds).
    bool even_turn = true;             //If true, player white uses custom timing heuristics. False -> black uses them.
    unsigned int depth_randomness = 0; //Allows player without timing heuristic to randomize depth.
    //Set to nonzero values to randomize games, ensuring non-deterministic results.


    // GAME START
    cout << " --- Game Start --- " << endl;
    while (move.src_i != -1 && time > 0)
    {
        t_time = clock();
        Lookup::UPDATE(time, (unsigned int)Instance.countPieces());
        try
        {
            //If "even_turn", white's move. If even_turn is false, black moves under stunted settings. In general,
            //player white should win. Hard-coding the depth and QS depth for player black shows the increased effect
            //time/depth heuristics offer.
            if(even_turn) Instance.display(); //Displays when player using time heuristics is evaluating moves.
            if(even_turn) move = Instance.TLHTQSID_ABDLMM_Algorithm(Lookup::MaximumDepth(), Lookup::QuiescentDepth());
            else move = Instance.TLHTQSID_ABDLMM_Algorithm((rand()%(depth_randomness+1)) + 1, 2);
            even_turn = !even_turn;
        }
        catch (Chessmove e)
        {
            cerr << "Invalid State." << endl;
            exit(1);
        }
        catch (...)
        {
            cerr << "UNKNOWN ERROR" << endl;
            exit(1);
        }
        if (move.src_i != -1) Instance.execute(move);
        cout << (time -= (clock() - t_time) * 1000) / 1000000.0 << " seconds remaining" << endl;
    }
    cout << " --- Game Complete --- " << endl << endl;
    cout << "Final State:" << endl;
    Instance.display();
    return 0;
}