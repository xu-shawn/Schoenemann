#pragma once
#include "movegen/chess.hpp"

using namespace chess;


constexpr int MATE_SCORE = 32766;

class searcher {
public:
	const short infinity = 32767;
	Move bestMove = Move::NULL_MOVE;

	bool isNormalSearch = true;
	int timeForMove = 0;
	bool shouldStop = false;

	int transpositions = 0;
	int nodes = 0;

	int pvs(int alpha, int beta, int depth, int ply, Board& board);
	int qs(int alpha, int beta, Board& board, int ply, int plies);
	void iterativeDeepening(Board& board);


	int scoreMate(bool isInCheck, int ply) 
	{
		if (isInCheck)
		{
			return (-MATE_SCORE + ply);
		}
		else
		{
			return 0;
		}
	}

	Move getBestMove()
	{
		return bestMove;
	}
private:
};
