#include "game.h"

using namespace std;

enum solution{ MINIMAX, ALFA_BETA, GOLOSA};

class Player {
    public:
        column calculateMove(solution h, int placedPieces);
        pair<score, column> minimax(int placedPieces, int maximizingPlayer);
        pair <score, column> minimaxABaux(int placedPieces, int maximizingPlayer, score alfa, score beta);
        pair <score, column> minimaxAB(int placedPieces);
        Player(Game game);
        void addPiece(column col, int player);
        void printBoard();
    private:
        Game game;
};

column Player::calculateMove(solution h, int placedPieces){

    if(h == MINIMAX) {
        return minimax(placedPieces, 1).second;
    }
    else if(h == ALFA_BETA){
        return minimaxAB(placedPieces).second;
    }
    return 0;

}

Player::Player(Game game):
    game(game) {
    }

void Player::addPiece(column col, int player){
    game.addPiece(col, player);
}
void Player::printBoard(){
    game.printBoard();
}

pair <score, column> Player::minimax(int placedPieces, int maximizingPlayer) {
    column free = game.firstFreeColumn();
    Winner winner = game.checkGame();
    if (winner != DRAW) {
        return make_pair(winner, free);
    }

    if (placedPieces == game.p) {
        // TODO
        return make_pair(winner, free);
    }
    int columns = game.getColumns();
    if (maximizingPlayer == 1){
        // int bestValue = std::numeric_limits<int>::min();
        int bestValue = PLAYER2;
        int bestMove = free;

        for ( column col = 0; col < columns; ++col) {
            if(game.isFree(col)){
                game.addPiece(col,1);
                pair <score, column> value = minimax(placedPieces + 1, -1);
                if (bestValue < value.first){
                    bestValue = value.first;
                    bestMove = col;
                }

                game.removePiece(col);
            }
        }
        return make_pair(bestValue, bestMove);
    }
    else {
        // int bestValue = std::numeric_limits<int>::max();
        int bestValue = PLAYER1;
        int bestMove = free;
        for ( column col = 0; col < columns; ++col) {
            if(game.isFree(col)){
                game.addPiece(col,-1);
                pair <score, column> value= minimax(placedPieces, 1);
                if (bestValue > value.first){
                    bestValue = value.first;
                    bestMove = col;
                }
                game.removePiece(col);
            }
        }
        return make_pair(bestValue, bestMove);
    }
}

pair <score, column> Player::minimaxABaux(int placedPieces, int maximizingPlayer, score alfa, score beta) {
    column free = game.firstFreeColumn();
    Winner winner = game.checkGame();

    if (winner != DRAW) {
        return make_pair(winner, free);
    }

    if (placedPieces == game.p) {
        // TODO
        return make_pair(winner, free);
    }

    int columns = game.getColumns();
    
    if (maximizingPlayer == 1){
        // Busca la movida óptima para el jugador que maximiza, es decir nosotros.
        int bestValue = PLAYER2; //Peor valor posible: que gane el oponente.
        int bestMove = free;

        for ( column col = 0; col < columns; ++col) {
            if(game.isFree(col)){
                game.addPiece(col,1);
                pair <score, column> value = minimaxABaux(placedPieces + 1, -1, alfa, beta);
                game.removePiece(col);
                if (bestValue < value.first){
                    bestValue = value.first;
                    bestMove = col;
                }
                if(alfa < value.first) alfa = value.first;
                if(beta <= alfa) break;
            }
        }
        return make_pair(bestValue, bestMove);
    }
    else {
        // Buscamos la movida óptima para el jugador que minimiza, es decir el oponente. Es importante aclarar que el oponente va a intentar hacer la mejor movida para él; somos nosotros los que queremos que bestValue sea lo menor posible y por eso elegimos nuestra jugada en base a eso.
        // int bestValue = std::numeric_limits<int>::max();
        int bestValue = PLAYER1;
        int bestMove = free;
        for ( column col = 0; col < columns; ++col) {
            if(game.isFree(col)){
                game.addPiece(col,-1);
                pair <score, column> value= minimaxABaux(placedPieces, 1, alfa, beta);
                game.removePiece(col);
                if (bestValue > value.first){
                    bestValue = value.first;
                    bestMove = col;
                }
                if(beta > value.first) beta = value.first;
                if(beta <= alfa) break;
            }
        }
        return make_pair(bestValue, bestMove);
    }
}

pair <score, column> Player::minimaxAB(int placedPieces){
    return minimaxABaux(placedPieces, 1, PLAYER2, PLAYER1);
}


//
//    01 function minimax(node, depth, maximizingPlayer)
//    02     if depth = 0 or node is a terminal node
//    03         return the heuristic value of node
//
//    04     if maximizingPlayer
//        05         bestValue := −∞
//    06         for each child of node
//    07             v := minimax(child, depth − 1, FALSE)
//    08             bestValue := max(bestValue, v)
//    09         return bestValue
//
//    10     else    (* minimizing player *)
//    11         bestValue := +∞
//    12         for each child of node
//    13             v := minimax(child, depth − 1, TRUE)
//    14             bestValue := min(bestValue, v)
//    15         return bestValue

