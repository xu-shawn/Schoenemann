#include <atomic>
#include <chrono>
#include <thread>
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


int search(int depth, int alpha, int beta, int ply, Board& board) 
{
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
        return quiescence_search(alpha, beta, board);
    }


    Movelist movelist;
    movegen::legalmoves(movelist, board);

    movelist = orderMoves(movelist, board, depth, ply, alpha, beta);

    if (movelist.size() == 0)
    {
        if (board.inCheck() == true) 
        {
            return -infinity;
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

int quiescence_search(int alpha, int beta, Board& board) 
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

std::atomic<bool> time_up(false);

void time_checker(int time_for_move, std::chrono::high_resolution_clock::time_point start)
{
    while (!time_up)
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        if (elapsed.count() >= time_for_move)
        {
            time_up = true;
        }
    }
}

void iterative_deepening(Board& board)
{
    auto start = std::chrono::high_resolution_clock::now();
    int time_for_move = get_time_for_move();
    bestMove = Move::NULL_MOVE;
    bool hasFoundMove = false;

    std::thread timer_thread(time_checker, time_for_move, start);

    for (int i = 1; i < 256 && !time_up; i++)
    {
        search(i, -32767, 32767, 0, board);
        if (time_up)
        {
            std::cout << "bestmove " << bestMove << std::endl;
        }
        else
        {
            std::cout << "info depth " << i << " moves " << bestMove << std::endl;
        }
    }

    timer_thread.join();
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
