#pragma once
#include "movegen/chess.hpp"

using namespace chess;

int getNodes();
void setNodes(int newNodes);
int search(int depth, int alpha, int beta, int ply, Board& board);
chess::Move& getBestMove();
int quiescence_search(int alpha, int beta, Board& board);
void iterative_deepening(Board& board);
int getTranspositions();
void time_checker(int time_for_move, std::chrono::high_resolution_clock::time_point start);
