#include <chrono>
#include <iostream>

#include "Search.h"
#include "evaluate.h"
#include "movegen/chess.hpp"
#include "timeman.h"
#include "Moveorder.h"
#include "consts.h"

using namespace chess;

std::chrono::time_point start = std::chrono::high_resolution_clock::now();

int searcher::pvs(int alpha, int beta, int depth, int ply, Board& board)
{

    if (shouldStop)
    {
        return alpha;
    }

    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    bool isOver = elapsed.count() >= timeForMove;
    if (isOver && !isNormalSearch)
    {
        shouldStop = true;
    }

	if (depth == 0)
	{
		return evaluate(board);
	}

	bool bSearchPv = true;

	Movelist moveList;
	movegen::legalmoves(moveList, board);


    if (moveList.size() == 0)
    {
        if (board.inCheck() == true)
        {
            return ply - infinity;
        }
        else
        {
            return 0;
        }
    }

	for (const Move& move : moveList)
	{
		board.makeMove(move);
		int score;
		if (bSearchPv)
		{
			score = -pvs(-beta, -alpha, depth - 1, ply + 1, board);
		}
		else
		{
			score = -pvs(-alpha - 1, -alpha, depth - 1, ply + 1, board);
			if (score > alpha && score < beta)
			{
				score = -pvs(-beta, -alpha, depth - 1, ply + 1, board);
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
			bSearchPv = false;
			if (ply == 0)
			{
				bestMove = move;
			}
		}
	}

	return alpha;
}

void searcher::iterativeDeepening(Board& board)
{
    start = std::chrono::high_resolution_clock::now();
    timeForMove = getTimeForMove();
    bestMove = Move::NULL_MOVE;
    isNormalSearch = false;
    bool hasFoundMove = false;

    if (timeForMove == -20)
    {
        pvs(-32767, 32767, 1, 0, board);
        if (bestMove != Move::NULL_MOVE)
        {
            std::cout << "bestmove " << bestMove << std::endl;
            return;
        }
    }

    for (int i = 1; i <= 256; i++)
    {
		pvs(-32767, 32767, i, 0, board);

        if (bestMove != Move::NULL_MOVE)
        {
            hasFoundMove = true;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        bool isOver = elapsed.count() >= timeForMove;

        //std::cout << "Time for this move: " << timeForMove << " | Time used: " << static_cast<int>(elapsed.count()) << " | Depth: " << i << " | bestmove: " << bestMove << std::endl;
        if (i == 256 && hasFoundMove)
        {
            std::cout << "bestmove " << bestMove << std::endl;
            break;
        }

        if (isOver && hasFoundMove)
        {
            std::cout << "bestmove " << bestMove << std::endl;
            shouldStop = true;
            break;
        }

    }
    shouldStop = false;
    isNormalSearch = true;
}
