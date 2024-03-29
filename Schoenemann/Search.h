#pragma once
#include "movegen/chess.hpp"

using namespace chess;

int getNodes();
int search(int depth, int alpha, int beta, int ply, Board& board);
chess::Move& getBestMove();
int quiescence_search(int alpha, int beta, Board& board);
