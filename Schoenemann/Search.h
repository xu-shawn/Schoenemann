#pragma once
#include "movegen/chess.hpp"

using namespace chess;

class searcher {
public:
	const short infinity = 32767;
	Move bestMove = Move::NULL_MOVE;

	bool isNormalSearch = true;
	int timeForMove = 0;
	bool shouldStop = false;

	int pvs(int alpha, int beta, int depth, int ply, Board& board);
	void iterativeDeepening(Board& board);

	Move getBestMove()
	{
		return bestMove;
	}
private:
};
