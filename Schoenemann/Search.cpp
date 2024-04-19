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
bool shouldStop = false;
bool isNormalSearch = true;
auto start = std::chrono::high_resolution_clock::now();
int timeForMove = 0;

int search(int depth, int alpha, int beta, int ply, Board& board)
{
    if (shouldStop)
    {
        return alpha;
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
        return quiescenceSearch(alpha, beta, ply, board);
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

    moveList = orderMoves(moveList, board, depth, ply, alpha, beta);

    int evalType = transpositionTabel.uppperBound;

    for (const Move& move : moveList)
    {
        count_nodes++;
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
            if (ply == 0)
            {
                bestMove = move;
            }
            alpha = score;
        }
    }

    transpositionTabel.storeEvaluation(depth, ply, alpha, evalType, bestMove, board);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    bool isOver = elapsed.count() >= timeForMove;
    if (isOver && !isNormalSearch)
    {
        //std::cout << "Time used: " << (std::chrono::high_resolution_clock::now() - start).count() << "Time for Move: " << timeForMove;
        shouldStop = true;
    }

    return alpha;
}

int quiescenceSearch(int alpha, int beta, int ply, Board& board) 
{

	int eval = evaluate(board);

	if (eval >= beta)
	{
		return beta;
	}

	if (alpha < eval)
	{
		alpha = eval;
	}

	Movelist moveList;
	movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moveList, board);
    if (board.inCheck())
    {
        return checkQuiescenceSearch(alpha, beta, ply, board);
    }

	for (const Move& move : moveList) 
    {
        board.makeMove(move);
        int score = -quiescenceSearch(-beta, -alpha, ply, board);
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


int checkQuiescenceSearch(int alpha, int beta, int ply, Board& board)
{
    Movelist moveList;
    movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moveList, board);
    int bestScore = -infinity;
    int score = -infinity;
    for (const Move& move : moveList)
    {
        board.makeMove(move);
        score = -quiescenceSearch(-beta, -alpha, ply, board);
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

void iterativeDeepening(Board& board)
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

    for (int i = 1; i < 256; i++)
    {
        if (bestMove != Move::NULL_MOVE)
        {
            hasFoundMove = true;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        bool isOver = elapsed.count() >= timeForMove;

        //std::cout << "Time for this move: " << timeForMove << " | Time used: " << elapsed.count() << std::endl;

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
