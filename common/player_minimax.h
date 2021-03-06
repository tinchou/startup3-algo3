#ifndef STARTUP3_ALGO3_PLAYER_MINIMAX_H
#define STARTUP3_ALGO3_PLAYER_MINIMAX_H

#include <algorithm>
#include <chrono>
#include "minimax.h"
#include "possible_move.h"
#include "player.h"

// using namespace chrono;

class PlayerMinimax : public Player {
public:
    int nextMove(Game& game) {
        // high_resolution_clock::time_point beginMove = high_resolution_clock::now();
        auto moves = game.possibleMoves();
        auto max = max_element(moves.begin(), moves.end(),
                               [game](PossibleMove& m1, PossibleMove& m2) {
                                   m1.in();
                                   auto res1 = minimax(m1, game.remainingPieces(), false);
                                   m1.out();
                                   m2.in();
                                   auto res2 = minimax(m2, game.remainingPieces(), false);
                                   m2.out();
                                   return res1 < res2;
                               }
        );

        // high_resolution_clock::time_point endMove = high_resolution_clock::now();

        // duration<double> timeFindingMove = duration_cast< duration<double> >(endMove - beginMove);

        assert(max != moves.end()); // encontró alguno

        // std::cerr << game.board().possibleMoves().size() << "; "<< timeFindingMove.count() << "; "<< game.board().columns() <<"; "<< game.board().rows() <<"; " << game.cToWin()<<endl;

        return max->move();
    }
};

#endif //STARTUP3_ALGO3_PLAYER_MINIMAX_H
