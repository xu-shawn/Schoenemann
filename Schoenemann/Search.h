#pragma once
#include "movegen/chess.hpp"

int getNodes();
int search(int depth, int alpha, int beta, chess::Board& board);
chess::Move& getBestMove();
