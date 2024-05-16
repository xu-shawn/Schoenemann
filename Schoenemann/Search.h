#pragma once
#include "movegen/chess.hpp"

using namespace chess;

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

class searcher {
public:
	const short infinity = 32767;
	Move bestMove = Move::NULL_MOVE;
	bool shouldStop = false;
	bool isNormalSearch = true;
	int timeForMove = 0;

	int transpositions = 0;
	int nodes = 0;

	int pvs(int alpha, int beta, int depth, int ply, Board& board);
	int qs(int alpha, int beta, Board& board, int ply, int depth);
	void iterativeDeepening(Board& board);
private:
};
