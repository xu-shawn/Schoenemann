#include "Search.h"
#include "Evaluate.h"
#include "movegen/chess.hpp"

using namespace chess;

const short infinity = 32767;
int count_nodes = 0;
int score;
chess::Move bestMove = chess::Move::NULL_MOVE;

int search(int depth, int alpha, int beta, Board& board) {
	if (depth == 0)
	{
		return evaluate(board);
	}
	
	Movelist movelist;
	movegen::legalmoves(movelist, board);

	if (movelist.size() == 0)
	{
		if (board.inCheck())
		{
			return infinity;
		}
		else
		{
			return 0;
		}
	}

	bool b_seach_pv = true;

	for (const auto& move : movelist) {
		count_nodes++;
		board.makeMove(move);
		if (b_seach_pv)
		{
			score = -search(depth - 1, -beta, -alpha, board);
		}
		else
		{
			score = -search(depth - 1, -alpha - 1, -alpha, board);
			if (score > alpha)
			{
				score - search(depth - 1, -beta, -alpha, board);
			}
		}
		board.unmakeMove(move);
		if (score >= beta)
		{
			return beta;
		}
		if (score > alpha)
		{
			alpha = score;
			b_seach_pv = false;
			bestMove = move;
		}
	}
	return alpha;
}

int getNodes() {
	return count_nodes;
}

chess::Move& getBestMove() {
	return bestMove;
}
