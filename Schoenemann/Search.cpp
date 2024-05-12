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

    HashEntry* entry = transpositionTabel.getHash(board);

    if (entry != nullptr)
    {
        if (entry->key == board.hash())
        {
            if (entry->depth >= depth)
            {
                uint8_t nodeType = entry->ageNodeType;
                if (entry->score >= beta && nodeType == CUT_NODE)
                {
                    transpositions++;
                    return entry->score;
                }
                else if (entry->score <= alpha && nodeType == ALL_NODE)
                {
                    transpositions++;
                    return entry->score;
                }
            }
            
        }
    }

    if (depth == 0)
    {
        return qs(alpha, beta, board, ply, 0, depth);
    }

    bool bSearchPv = true;

    Movelist moveList;
    movegen::legalmoves(moveList, board);


    if (moveList.size() == 0)
    {
        if (board.inCheck() == true)
        {
            return scoreMate(board.inCheck(), ply);
        }
        else
        {
            return 0;
        }
    }

    for (const Move& move : moveList)
    {
        board.makeMove(move);
        nodes++;
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
            transpositionTabel.storeEvaluation(board, transpositionTabel.adjustHashScore(score, ply), move, score, depth, CUT_NODE);
            return beta;
        }

        if (score > alpha)
        {
            alpha = score;
            transpositionTabel.storeEvaluation(board, transpositionTabel.adjustHashScore(score, ply), move, score, depth, ALL_NODE);
            bSearchPv = false;
            if (ply == 0)
            {
                bestMove = move;
            }
        }
    }

    return alpha;
}

int searcher::qs(int alpha, int beta, Board& board, int ply, int plies, int depth)
{

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

    for (const Move& move : moveList)
    {
        board.makeMove(move);

        int score = -qs(-beta, -alpha, board, ply, plies + 1, depth);

        board.unmakeMove(move);

        if (score >= beta)
        {
            //transpositionTabel.storeEvaluation(board, transpositionTabel.adjustHashScore(score, ply), move, score, -plies, CUT_NODE);
            return beta;
        }

        if (score > alpha)
        {
            alpha = score;
        }
    }

    return alpha;
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

    transpositionTabel.incrementAge();

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