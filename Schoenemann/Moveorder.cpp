#include "Moveorder.h"
#include "consts.h"

Movelist orderMoves(Movelist moveList, Board& board, int depth, int ply, int alpha, int beta)
{
	Move hashMove = transpositionTabel.getStoredMove(board);
	for (Move move : moveList)
	{
		if (move == hashMove)
		{
			if (hashMove != Move::NO_MOVE && hashMove != Move::NULL_MOVE)
			{
				Move cache = moveList.front();
				moveList.front() = hashMove;
				moveList.back() = cache;
			}
		}
	}
	return moveList;
}