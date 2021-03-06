#ifndef STARTUP3_ALGO3_PLAYER_GENETIC_H
#define STARTUP3_ALGO3_PLAYER_GENETIC_H

#include <algorithm>
#include "../common/player.h"
#include "../common/possible_move.h"
#include "genome.h"


class PlayerGenetic : public Player {
private:
    Genome g;
public:
    PlayerGenetic(Genome g) : g(g) {}

    int nextMove(Game &game) override {

        auto moves = game.possibleMoves();
        auto bestCol = max_element(moves.begin(), moves.end(),
                                   [this, game](PossibleMove& m1, PossibleMove& m2) {
                                       return g.activate(game.board(), m1) < g.activate(game.board(), m2);
                                   }
        );
        assert(bestCol != moves.end()); // encontró alguno

        // cerr << "PossibleMoves: " << endl;
        // for (auto i : game.board().possibleMoves())
        // {
        //     cerr << i <<" ";
        // }

        // cerr << endl << "Genetic elige: " << *bestCol << endl;

        return bestCol->move();
    }
};

#endif //STARTUP3_ALGO3_PLAYER_GENETIC_H
