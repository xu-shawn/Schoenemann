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

    if (depth <= 0 || ply >= 127)
    {
        return qs(alpha, beta, board, ply, 0);
    }

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

    // Mate distance pruning
    int matingScore = MATE_SCORE - ply;
    if (matingScore < beta) 
    {
        beta = matingScore;
        if (alpha >= matingScore)
        {
            return alpha;
        }
    }

    int matedScore = -MATE_SCORE + ply;
    if (matedScore > alpha) 
    {
        alpha = matedScore;
        if (beta <= matedScore)
        {
            return beta;
        }
    }

    bool isPVNode = (beta - alpha != 1);

    Move hashed = Move::NULL_MOVE;

    int hashScore = -infinity;
    int hashDepth = 0;

    uint8_t nodeType = NO_NODE_INFO;

    //The previos alpha is our current alpha
    int prevAlpha = alpha;

    HashEntry* hashEntry = transpositionTabel.getHash(board);
    if (hashEntry != nullptr) {

        //Get all the informations out of the entry
        hashScore = hashEntry->score;
        nodeType = hashEntry->ageNodeType & 0x3;
        hashDepth = hashEntry->depth;
        hashed = hashEntry->move;

        if (hashScore != -infinity) 
        {
            //Correct the score of the hash if it is a mate score
            if (hashScore >= MAX_PLY_MATE_SCORE)
            {
                hashScore -= ply;
            }
            else if (hashScore <= -MAX_PLY_MATE_SCORE)
            {
                hashScore += ply;
            }

            if (!isPVNode && hashDepth >= depth) 
            {
                if ((nodeType == ALL_NODE && hashScore <= alpha) || (nodeType == CUT_NODE && hashScore >= beta) || (nodeType == PV_NODE)) 
                {
                    return hashScore;
                }
            }
        }
    }


    int staticEval = infinity;

    if (!board.inCheck())
    {
        //Checks for a saved position
        if (hashEntry != nullptr && hashEntry->eval != infinity) 
        {
            staticEval = hashEntry->eval;
        }
        else 
        {
            //Store the evaluation if no position was found
            staticEval = (board.sideToMove() == Color::WHITE) ? evaluate(board) : -evaluate(board);
            transpositionTabel.storeEvaluation(board, -infinity, Move::NULL_MOVE, staticEval, -8, NO_NODE_INFO);
        }
    }

    //Use the hashed transposition score
    if (hashScore != -infinity && staticEval != infinity) 
    {
        if ((nodeType == ALL_NODE && hashScore < staticEval) || (nodeType == CUT_NODE && hashScore > staticEval) || (nodeType == PV_NODE))
        {
            staticEval = hashScore;
        }
            
    }

    if (hashed == Move::NULL_MOVE && !board.inCheck() && ((isPVNode && depth >= 6) || (!isPVNode && depth >= 8))) 
    {
        int iidDepth = isPVNode ? depth - depth / 4 - 1 : (depth - 5) / 2;
        pvs(alpha, beta, iidDepth, ply, board);

        HashEntry* iidEntry = transpositionTabel.getHash(board);
        if (iidEntry != nullptr) 
        {
            hashScore = iidEntry->score;
            nodeType = iidEntry->ageNodeType & 0x3;
            hashDepth = iidEntry->depth;
            hashed = iidEntry->move;
        }
    }

	Movelist moveList;
	movegen::legalmoves(moveList, board);
    int bestScore = -infinity;
    int score = -infinity;
    Move toHash = Move::NULL_MOVE;

	for (const Move& move : moveList)
	{
		board.makeMove(move);
        nodes++;
		score = -pvs(-alpha - 1, -alpha, depth - 1, ply + 1, board);

        //Null Windows Search
		if (beta == alpha + 1)
		{
			score = -pvs(-beta, -alpha, depth - 1, ply + 1, board);
		}

		board.unmakeMove(move);

		if (score >= beta)
		{
            transpositionTabel.storeEvaluation(board, transpositionTabel.adjustHashScore(score, ply), move, staticEval, depth, CUT_NODE);
			return score;
		}

        if (score > bestScore)
        {
            bestScore = score;
            if (score > alpha)
            {
                alpha = score;
                toHash = move;
                bestMove = move;
            }
        }
	}

    if (bestScore == -infinity)
    {
        return scoreMate(board.inCheck(), ply);
    }

    // Exact scores indicate a principal variation
    if (prevAlpha < alpha && alpha < beta) 
    {
        transpositionTabel.storeEvaluation(board, transpositionTabel.adjustHashScore(alpha, ply), toHash, staticEval, depth, PV_NODE);
    }

    //Store the All NODE
    else if (alpha <= prevAlpha) 
    {
        // If we had a hash move, save it in case the node becomes a PV or cut node next time
        if (!isPVNode && hashed != Move::NULL_MOVE) 
        {
            transpositionTabel.storeEvaluation(board, transpositionTabel.adjustHashScore(bestScore, ply), hashed, staticEval, depth, ALL_NODE);
        }
        // Otherwise, just store no best move as expected
        else 
        {
            transpositionTabel.storeEvaluation(board, transpositionTabel.adjustHashScore(bestScore, ply), Move::NULL_MOVE, staticEval, depth, ALL_NODE);
        }
    }

	return bestScore;
}

int searcher::qs(int alpha, int beta, Board& board, int ply, int plies)
{

    int hashScore = -infinity;
    HashEntry  *hashEntry = transpositionTabel.getHash(board);
    uint8_t nodeType = NO_NODE_INFO;
    if (hashEntry != nullptr) 
    {
        hashScore = hashEntry->score;

        if (hashScore != -infinity) 
        {
            //Adjust the score if it is a mate score
            if (hashScore >= MAX_PLY_MATE_SCORE)
            {
                hashScore -= ply + plies;
            }
            else if (hashScore <= -MAX_PLY_MATE_SCORE)
            {
                hashScore += ply + plies;
            }


            //Get the node type
            nodeType = hashEntry->ageNodeType & 0x3;

            if (hashEntry->depth >= -plies) 
            {
                if ((nodeType == ALL_NODE && hashScore <= alpha) || (nodeType == CUT_NODE && hashScore >= beta) || (nodeType == PV_NODE))
                {
                    return hashScore;
                }
            }
        }
    }

    //Transposition preperations for standPat

    int hashEval;
    int staticEval;

    if (hashEntry != nullptr)
    {
        if (hashEntry->eval != infinity)
        {
            hashEval = staticEval = hashEntry->eval;
        }
        else
        {
            hashEval = staticEval = (board.sideToMove() == Color::WHITE) ? evaluate(board) : -evaluate(board);
        }
    }
    else
    {
        //Simply store the evaluation
        hashEval = staticEval = (board.sideToMove() == Color::WHITE) ? evaluate(board) : -evaluate(board);
        transpositionTabel.storeEvaluation(board, -infinity, Move::NULL_MOVE, hashEval, -8, NO_NODE_INFO);
    }

    if (hashScore != -infinity) {
        if ((nodeType == ALL_NODE && hashScore < staticEval) || (nodeType == CUT_NODE && hashScore > staticEval) || (nodeType == PV_NODE))
        {
            staticEval = hashScore;
            transpositions++;
        }
    }


    //Stand pat
    if (staticEval >= beta)
    {
        return staticEval;
    }

    if (alpha < staticEval)
    {
        alpha = staticEval;
    }

    Movelist moveList;
    movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moveList, board);

    int bestScore = staticEval;

    for (const Move& move : moveList)
    {
        board.makeMove(move);

        int score = -qs(-beta, -alpha, board, ply, plies + 1);

        board.unmakeMove(move);

        

        if (score >= beta)
        {
            transpositionTabel.storeEvaluation(board, transpositionTabel.adjustHashScore(score, ply + plies), move, hashEval, -plies, CUT_NODE);
            return beta;
        }

        if (score > bestScore)
        {
            bestScore = score;
            if (score > alpha)
            {
                alpha = score;
            }
        }
    }

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

    transpositionTabel.incrementAge();

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
