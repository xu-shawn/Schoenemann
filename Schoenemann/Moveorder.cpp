#include "Moveorder.h"
#include "consts.h"

Movelist orderMoves(Movelist moveList, Board& board, int depth, int ply, int alpha, int beta)
{
	if (transpositionTabel.lookUpEvaluation(depth, ply, alpha, beta, board) != -40000)
	{
		std::cout << "hi";
		Move ttMove = transpositionTabel.getStoredMove(board);
		if (ttMove != Move::NO_MOVE && ttMove != Move::NULL_MOVE)
		{
			Move cache = moveList.front();
			moveList.front() = ttMove;
			moveList.back() = cache;
		}
	}

	return moveList;
}