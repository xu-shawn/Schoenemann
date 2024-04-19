#pragma once
#include "movegen/chess.hpp"

using namespace chess;

int getNodes();
void setNodes(int newNodes);
int search(int depth, int alpha, int beta, int ply, Board& board);
chess::Move& getBestMove();
int quiescenceSearch(int alpha, int beta, int ply, Board& board);
void iterativeDeepening(Board& board);
int getTranspositions();
int checkQuiescenceSearch(int alpha, int beta, int ply, Board& board);
