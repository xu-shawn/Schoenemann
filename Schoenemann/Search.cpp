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
    int hashedScore = 0;
    const bool pvNode = alpha != beta - 1;
    const bool root = ply == 0;
    Hash* entry = transpositionTabel.getHash(board);
    if (entry != nullptr)
    {
        if (board.hash() == entry->key)
        {
            hashedScore = transpositionTabel.ScoreFromTT(entry->score, ply);
        }
    }

    if (entry != nullptr)
    {
        if (!pvNode && entry->depth >= depth)
        {
            transpositions++;
            return hashedScore;
        }
    }

    short type = ALPHA;

    if (depth == 0)
    {
        return qs(alpha, beta, board, ply, depth);
    }

    bool bSearchPv = true;

    Movelist moveList;
    movegen::legalmoves(moveList, board);


    if (moveList.size() == 0)
    {
        if (board.inCheck() == true)
        {
            return -MATE + ply;
        }
        else
        {
            return 0;
        }
    }
    int score;
    int bestScore = -infinity;
    for (const Move& move : moveList)
    {
        board.makeMove(move);
        nodes++;
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

        if (score > bestScore)
        {
            bestScore = score;
            if (score > alpha)
            {
                alpha = score;
                bSearchPv = false;
                type = EXACT;
                if (ply == 0)
                {
                    bestMove = move;
                }
            }
            if (score >= beta)
            {
                break;
            }
        }
    }
    //bestScore = MIN(bestScore, infinity);

    transpositionTabel.storeEvaluation(board.hash(), depth, type, transpositionTabel.ScoreToTT(bestScore, ply), bestMove);

    return bestScore;
}

int searcher::qs(int alpha, int beta, Board& board, int ply, int depth)
{
    Hash* entry = transpositionTabel.getHash(board);

    int hashedScore = 0;
    const bool pvNode = alpha != beta - 1;
    if (entry != nullptr)
    {
        if (board.hash() == entry->key)
        {
            hashedScore = transpositionTabel.ScoreFromTT(entry->score, ply);
        }
    }

    if (entry != nullptr)
    {
        if (!pvNode && entry->depth >= depth)
        {
            transpositions++;
            return hashedScore;
        }
    }

    int standPat = evaluate(board);

    if (standPat >= beta)
    {
        return beta;
    }

    if (alpha < standPat)
    {
        alpha = standPat;
    }

    Movelist moveList;
    movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moveList, board);

    int bestScore = standPat;

    for (const Move& move : moveList)
    {
        board.makeMove(move);

        int score = -qs(-beta, -alpha, board, ply, depth);

        board.unmakeMove(move);

        if (score > bestScore)
        {
            bestScore = score;

            if (score > alpha)
            {
                alpha = score;
            }

            if (score >= beta)
            {
                break;
            }
        }
    }

    if (board.inCheck() && bestScore == -infinity)
    {
        return -MATE + ply;
    }

    //transpositionTabel.storeEvaluation(board.hash(), depth, EXACT, transpositionTabel.ScoreToTT(bestScore, ply), Move::NULL_MOVE);

    return bestScore;
}

void searcher::iterativeDeepening(Board& board)
{
    start = std::chrono::high_resolution_clock::now();
    timeForMove = getTimeForMove();
    bestMove = Move::NULL_MOVE;
    Move bestMoveThisIteration = Move::NULL_MOVE;
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

        if (!shouldStop)
        {
            bestMoveThisIteration = bestMove;
        }

        if (bestMoveThisIteration == Move::NULL_MOVE)
        {
            bestMoveThisIteration = bestMove;
        }

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
            std::cout << "bestmove " << bestMoveThisIteration << std::endl;
            shouldStop = true;
            break;
        }

    }
    shouldStop = false;
    isNormalSearch = true;
}