#include <iostream>

#include "tt.h"

#include "chess.hpp"
#include "consts.h"
#include "tt.h"

using namespace chess;

void orderMoves(Movelist moveList, Hash* entry, Board& board, int *scores);
Move sortByScore(Movelist moveList, int *scores, int i);
const int hashMoveScore = 100000, promotion = 70000, goodCapture = 50000, badCapture = -50000;
