#include "Search.h"
#include "Evaluate.h"
#include "movegen/chess.hpp"

using namespace chess;

const short infinity = 32767;
int count_nodes = 0;
chess::Move bestMove = chess::Move::NULL_MOVE;

int search(int depth, int alpha, int beta, Board& board) {
    if (depth == 0)
    {
        return quiescence_search(alpha, beta, board);
    }

    Movelist movelist;
    movegen::legalmoves(movelist, board);
    for (const auto& move : movelist) {
        count_nodes++;
        board.makeMove(move);
        int evaluation = -search(depth - 1, -beta, -alpha, board);
        board.unmakeMove(move);
        if (evaluation >= beta)
        {
            return beta;
        }

        if (evaluation > alpha)
        {
            bestMove = move;
            alpha = evaluation;
        }
    }
    return alpha;
}

int quiescence_search(int alpha, int beta, Board& board) {
	int stand_pat = evaluate(board);
	if (stand_pat >= beta)
	{
		return beta;
	}
	if (alpha < stand_pat)
	{
		alpha = stand_pat;
	}

	Movelist movelist;
	movegen::legalmoves<movegen::MoveGenType::CAPTURE>(movelist, board);
	for (const auto& move : movelist) {
        board.makeMove(move);
        int score = -quiescence_search(-beta, -alpha, board);
        board.unmakeMove(move);
        if (score >= beta)
        {
            return beta;
        }
        if (score > alpha)
        {
            alpha = score;
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
