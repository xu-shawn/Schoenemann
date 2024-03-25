#include "Search.h"
#include "Evaluate.h"
#include "movegen/chess.hpp"
int bestEvaluation = 0;
short infinity = 32767;
chess::Move bestMove;
using namespace chess;

int search(int depth, int alpha, int beta, Board& board) {
	if (depth == 0)
	{
		return evaluate(board);
	}

	int c = 0;

	if (board.sideToMove() == Color::WHITE)
	{
		
		Movelist movelist;
		movegen::legalmoves(movelist, board);
		//MoveList<WHITE> moveList(position);
		if (movelist.size() == 0)
		{
			/*
			if (!(board.isGameOver() == GameResultReason::NONE))
			{
				return -infinity;
			}
			*/
			return 0;
		}
		for (const auto& move : movelist) {
			c++;
			std::cout << c << "\n" << std::endl;
			//std::cout << move;
			board.makeMove(move);
			int evaluation = -search(depth - 1, -beta, -alpha, board);
			board.unmakeMove(move);
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
		Movelist movelist;
		movegen::legalmoves(movelist, board);
		//MoveList<WHITE> moveList(position);
		if (movelist.size() == 0)
		{
			/*
			if (!(board.isGameOver() == GameResultReason::NONE))
			{
				return -infinity;
			}
			*/
			return 0;
		}
		for (const auto& move : movelist) {
			board.makeMove(move);
			int evaluation = -search(depth - 1, -beta, -alpha, board);
			board.unmakeMove(move);
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

chess::Move& getBestMove() {
	return bestMove;
}
