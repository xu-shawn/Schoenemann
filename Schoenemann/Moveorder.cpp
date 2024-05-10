#include "Moveorder.h"
#include "consts.h"

Movelist orderMoves(Movelist moveList, Board& board)
{
	/*
	Move hashMove = transpositionTabel.getEntry(board).move;

	if (hashMove != Move::NO_MOVE && hashMove != Move::NULL_MOVE)
	{
		for (Move move : moveList)
		{
			if (move == hashMove)
			{
				Move cache = moveList.front();
				moveList.front() = hashMove;
				moveList.back() = cache;
				break;
			}
		}
	}
	*/
	return moveList;
}