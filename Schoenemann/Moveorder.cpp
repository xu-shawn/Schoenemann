#include "Moveorder.h"
#include "consts.h"

Movelist orderMoves(Movelist moveList, Board& board)
{
	Move hashMove = transpositionTabel.getEntry(board).move;
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
			break;
		}
	}
	return moveList;
}