#include <chrono>
#include <iostream>

#include "Search.h"
#include "Evaluate.h"
#include "movegen/chess.hpp"
#include "timeman.h"
#include "Moveorder.h"
#include "consts.h"

using namespace chess;

const short infinity = 32767;
int count_nodes = 0;
int transpositions = 0;
chess::Move bestMove = chess::Move::NULL_MOVE;
std::chrono::high_resolution_clock::time_point start;
int timeForMove;


int search(int depth, int alpha, int beta, int ply, Board& board) 
{
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    bool isOver = elapsed.count() >= timeForMove;

    if (isOver && bestMove != Move::NULL_MOVE)
    {
        return -10000;
    }

    int ttEval = transpositionTabel.lookUpEvaluation(depth, ply, alpha, beta, board);

    if (ttEval != -40000)
    {
        transpositions++;
        if (ply == 0)
        {
            bestMove == transpositionTabel.getStoredMove(board);
        }
        return ttEval;
    }

    if (depth == 0)
    {
        return quiescenceSearch(alpha, beta, board);
    }


    Movelist movelist;
    movegen::legalmoves(movelist, board);

    movelist = orderMoves(movelist, board, depth, ply, alpha, beta);

    if (movelist.size() == 0)
    {
        if (board.inCheck() == true) 
        {
            return infinity;
        }
        else
        {
            return 0;
        }
    }

    int evalType = transpositionTabel.uppperBound;

    for (const auto& move : movelist) 
    {
        count_nodes++;
        board.makeMove(move);
        int evaluation = -search(depth - 1, -beta, -alpha, ply + 1, board);
        board.unmakeMove(move);

        if (evaluation >= beta)
        {
            transpositionTabel.storeEvaluation(depth, ply, beta, transpositionTabel.lowerBound, move, board);
            return beta;
        }

        if (evaluation > alpha)
        {
            evalType = transpositionTabel.exact;
            if (ply == 0)
            {
                bestMove = move;
            }
            alpha = evaluation;
        }
    }

    transpositionTabel.storeEvaluation(depth, ply, alpha, evalType, bestMove, board);

    return alpha;
}

int quiescenceSearch(int alpha, int beta, Board& board) 
{
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
	for (const auto& move : movelist) 
    {
        board.makeMove(move);
        int score = -quiescenceSearch(-beta, -alpha, board);
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

void iterativeDeepening(Board& board)
{
    start = std::chrono::high_resolution_clock::now();
    timeForMove = getTimeForMove();
    bestMove = Move::NULL_MOVE;
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

    for (int i = 1; i < 256; i++)
    {
        if (bestMove != Move::NULL_MOVE)
        {
            hasFoundMove = true;
        }

        search(i, -32767, 32767, 0, board);

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        bool isOver = elapsed.count() >= timeForMove;

        if (isOver && hasFoundMove)
        {
            std::cout << "bestmove " << bestMove << std::endl;
            break;
        }
    }
}




int getNodes() 
{
	return count_nodes;
}

int getTranspositions()
{
    return transpositions;
}

void setNodes(int newNodes)
{
    count_nodes = newNodes;
}

chess::Move& getBestMove() 
{
	return bestMove;
}
