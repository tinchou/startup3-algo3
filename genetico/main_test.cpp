#include <iostream>
#include "matingpool.h"

using namespace std;

int main() {
    Board board(6, 7);
    AmountOfFreeLinesOfLengthKGene gene(3);
    cerr << gene.boardProperty(board, board.getLowestFreeCell(3), 3) << endl;
    Genome genome(4);
    cout << genome.activate(board, 2) << endl;
	return 0;
}