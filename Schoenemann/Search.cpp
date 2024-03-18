#include "Search.h"
#include "Evaluate.h"
#include "movegen/position.h"
#include "movegen/tables.h"
#include "movegen/types.h"

int bestEvaluation = 0;
short infinity = 32767;
Move bestMove;

int search(int depth, int alpha, int beta, Position& position) {
	if (depth == 0)
	{
		return evaluate(position);
	}
	if (position.turn() == WHITE)
	{
		MoveList<WHITE> moveList(position);
		if (moveList.size() == 0)
		{
			if (position.in_check<WHITE>() == true)
			{
				return -infinity;
			}
			return 0;
		}
		for (const auto& move : moveList) {
			position.play<WHITE>(move);
			int evaluation = -search(depth - 1, -beta, -alpha, position);
			position.undo<WHITE>(move);
			if (evaluation >= beta)
			{
				return beta;
			}
			if (evaluation > alpha)
			{
				alpha = evaluation;
				if (depth == depth) {
					bestMove = move;
				}
			}
		}
		return alpha;

	}
	else
	{
		MoveList<BLACK> moveList(position);
		if (moveList.size() == 0)
		{
			if (position.in_check<BLACK>() == true)
			{
				return -infinity;
			}
			return 0;
		}
		for (const auto& move : moveList) {
			position.play<BLACK>(move);
			int evaluation = -search(depth - 1, -beta, -alpha, position);
			position.undo<BLACK>(move);
			if (evaluation >= beta)
			{
				return beta;
			}
			if (evaluation > alpha)
			{
				alpha = evaluation;
				if (depth == depth) {
					bestMove = move;
				}
			}
		}
		return alpha;
	}
	return bestEvaluation;
}

Move& getBestMove() {
	return bestMove;
}
