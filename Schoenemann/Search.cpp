#include <chrono>
#include <iostream>

#include "Search.h"
#include "Evaluate.h"
#include "movegen/chess.hpp"
#include "timeman.h"
#include "Moveorder.h"
#include "consts.h"

using namespace chess;

std::chrono::time_point start = std::chrono::high_resolution_clock::now();

int searcher::search(int depth, int alpha, int beta, int ply, Board& board)
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

    int ttEval = transpositionTabel.lookUpEvaluation(depth, ply, alpha, beta, board);

    if (ttEval != transpositionTabel.lookupFaild)
    {
        transpositions++;
        tt::entry hashEntry = transpositionTabel.getEntry(board);
        if (ply == 0)
        {
            bestMove = hashEntry.move;
        }
        return hashEntry.score;
    }

    if (depth == 0)
    {
        return quiescenceSearch(depth, alpha, beta, ply, board);
    }

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

    moveList = orderMoves(moveList, board);

    int evalType = transpositionTabel.uppperBound;

    Move bestMoveThisIteration = Move::NULL_MOVE;

    for (const Move& move : moveList)
    {
        board.makeMove(move);
        int score = -search(depth - 1, -beta, -alpha, ply + 1, board);
        board.unmakeMove(move);

        if (score >= beta)
        {
            transpositionTabel.storeEvaluation(depth, ply, beta, transpositionTabel.lowerBound, move, board);
            return beta;
        }

        if (score > alpha)
        {
            evalType = transpositionTabel.exact;
            bestMoveThisIteration = move;
            if (ply == 0)
            {
                bestMoveThisIteration = move;
                bestMove = move;
            }
            alpha = score;
        }
    }

    transpositionTabel.storeEvaluation(depth, ply, alpha, evalType, bestMoveThisIteration, board);

    return alpha;
}

int searcher::quiescenceSearch(int depth, int alpha, int beta, int ply, Board& board)
{
    if (transpositionTabel.lookUpEvaluation(depth, ply, alpha, beta, board) != -1)
    {
        if (ply == 0)
        {
            tt::entry hashEntry = transpositionTabel.getEntry(board);
            return hashEntry.score;
        }
    }

    if (board.inCheck())
    {
        return checkQuiescenceSearch(depth, alpha, beta, ply, board);
    }

	int score = evaluate(board);

	if (score >= beta)
	{
		return beta;
	}

	if (alpha < score)
	{
		alpha = score;
	}

	Movelist moveList;
	movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moveList, board);

	for (const Move& move : moveList) 
    {
        board.makeMove(move);
        int score = -quiescenceSearch(depth, -beta, -alpha, ply, board);
        board.unmakeMove(move);

        if (score >= beta)
        {
            transpositionTabel.storeEvaluation(depth, ply, beta, transpositionTabel.lowerBound, move, board);
            return beta;
        }
        if (score > alpha)
        {
            alpha = score;
        }
	}
	return alpha;
}


int searcher::checkQuiescenceSearch(int depth, int alpha, int beta, int ply, Board& board)
{
    Movelist moveList;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moveList, board);
    int bestScore = -infinity;
    int score = -infinity;
    count_nodes++;
    for (const Move& move : moveList)
    {
        if (bestScore > ply - infinity && !board.isCapture(move))
        {
            continue;
        }
        board.makeMove(move);
        score = -quiescenceSearch(depth, -beta, -alpha, ply, board);
        board.unmakeMove(move);

        if (score >= beta)
        {
            return score;
        }
            

        if (score > bestScore) {
            bestScore = score;
            if (score > alpha)
            {
                alpha = score;
            }
        }
    }

    if (bestScore == -infinity) {
        return ply - infinity;
    }

    return bestScore;
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
        search(1, -32767, 32767, 0, board);
        if (bestMove != Move::NULL_MOVE)
        {
            std::cout << "bestmove " << bestMove << std::endl;
            return;
        }
    }

    for (int i = 1; i <= 256; i++)
    {
        if (bestMove != Move::NULL_MOVE)
        {
            hasFoundMove = true;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        bool isOver = elapsed.count() >= timeForMove;

        //std::cout << "Time for this move: " << timeForMove << " | Time used: " << static_cast<int>(elapsed.count()) << " | Depth: " << i << " | bestmove: " << bestMove << std::endl;

        if (isOver && hasFoundMove)
        {
            std::cout << "bestmove " << bestMove << std::endl;
            shouldStop = true;
            break;
        }
        search(i, -32767, 32767, 0, board);
    }
    shouldStop = false;
    isNormalSearch = true;
}

int searcher::getNodes()
{
	return count_nodes;
}

int searcher::getTranspositions()
{
    return transpositions;
}

void searcher::setNodes(int newNodes)
{
    count_nodes = newNodes;
}

chess::Move& searcher::getBestMove()
{
	return bestMove;
}
