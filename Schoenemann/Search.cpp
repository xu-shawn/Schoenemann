#include <chrono>
#include <iostream>

#include "search.h"
#include "chess.hpp"
#include "timeman.h"
#include "moveorder.h"
#include "consts.h"
#include "nnue.hpp"

using namespace chess;

std::chrono::time_point start = std::chrono::high_resolution_clock::now();

int Search::pvs(int alpha, int beta, int depth, int ply, Board& board)
{
    //Increment nodes by one
    nodes++;

    if (shouldStop)
    {
        return beta;
    }

    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    bool isOver = elapsed.count() >= timeForMove;

    if (isOver && !isNormalSearch)
    {
        shouldStop = true;
    }

    //If depth is 0 we drop into qs to get a neutral position
    if (depth == 0)
    {
        return qs(alpha, beta, board, ply);
    }


    const std::uint64_t zobristKey = board.zobrist();

    if (ply > 0)
    {
        if (board.isHalfMoveDraw())
        {
            return 0;
        }

        if (contains(zobristKey))
        {
            return 0;
        }
    }

    int hashedScore = 0;
    short hashedType = 0;
    int hashedDepth = 0;
    int staticEval = NO_VALUE;

    //Get some important search constants
    const bool pvNode = (alpha != beta) - 1;
    const bool root = (ply == 0);

    //Get an potential hash entry
    Hash* entry = transpositionTabel.getHash(zobristKey);

    //Check if we this stored position is valid
    const bool isNullptr = entry == nullptr ? true : false;

    if (!isNullptr)
    {
        //If we have a transposition
        //That means that the current board zobrist key 
        //is the same as the hash entry zobrist key 
        if (zobristKey == entry->key)
        {
            hashedScore = transpositionTabel.ScoreFromTT(entry->score, ply);
            hashedType = entry->type;
            hashedDepth = entry->depth;
            staticEval = entry->eval;
        }

        //Check if we can return a stored score
        if (!pvNode && hashedDepth >= depth && transpositionTabel.checkForMoreInformation(hashedType, hashedScore, beta))
        {
            if ((hashedType == EXACT) ||
                (hashedType == UPPER_BOUND && hashedScore <= alpha) ||
                (hashedType == LOWER_BOUND && hashedScore >= beta))
            {
                return hashedScore;
            }
        }
    }

    if (!isNullptr)
    {
        if (zobristKey != entry->key && !board.inCheck() && depth >= 4)
        {
            depth--;
        }
    }

    if (!isNullptr)
    {
        int probCutBeta = beta + 390;
        if (hashedDepth >= depth - 2 && hashedScore >= probCutBeta && std::abs(beta) < infinity)
        {
            return probCutBeta;
        }
    }

    //If no evaluation was found in the transposition table
    //we perform an static evaulation
    if (staticEval == NO_VALUE)
    {
        staticEval = network.Evaluate((int)board.sideToMove());
    }

    //Reverse futility pruning
    if (!pvNode && !board.inCheck() && depth <= 6 && staticEval - 70 * depth >= beta)
    {
        return staticEval;
    }

    short type = UPPER_BOUND;

    bool bSearchPv = true;

    Movelist moveList;
    movegen::legalmoves(moveList, board);

    //Sort the list
    moveList = orderMoves(moveList, entry);

    if (moveList.size() == 0)
    {
        if (board.inCheck() == true)
        {
            return -infinity + ply;
        }
        else
        {
            return 0;
        }
    }
    int score = 0;
    int bestScore = -infinity;
    for (Move& move : moveList)
    {
        board.makeMove(move);

        short checkExtension = 0;

        if (board.inCheck() == true)
        {
            checkExtension = 1;
        }

        if (bSearchPv)
        {
            score = -pvs(-beta, -alpha, depth - 1 + checkExtension, ply + 1, board);
        }
        else
        {
            score = -pvs(-alpha - 1, -alpha, depth - 1 + checkExtension, ply + 1, board);
            if (score > alpha && score < beta)
            {
                score = -pvs(-beta, -alpha, depth - 1 + checkExtension, ply + 1, board);
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

                //If we are ate the root we set the bestMove
                if (ply == 0)
                {
                    bestMove = move;
                }
            }

            //Beta cutoff
            if (score >= beta)
            {
                break;
            }
        }
    }

    if (!root)
    {
        short finalType;
        //Calculate the node type
        if (bestScore >= beta)
        {
            finalType = LOWER_BOUND;
        }
        else if (pvNode && (type == EXACT))
        {
            finalType = EXACT;
        }
        else
        {
            finalType = UPPER_BOUND;
        }
        transpositionTabel.storeEvaluation(zobristKey, depth, finalType, transpositionTabel.ScoreToTT(bestScore, ply), bestMove, staticEval);
    }

    return bestScore;
}

int Search::qs(int alpha, int beta, Board& board, int ply)
{
    nodes++;
    const bool pvNode = (alpha != beta) - 1;
    const std::uint64_t zobristKey = board.zobrist();

    Hash* entry = transpositionTabel.getHash(zobristKey);
    const bool isNullptr = entry == nullptr ? true : false;

    int hashedScore = 0;
    short hashedType = 0;
    int standPat = NO_VALUE;

    if (!isNullptr)
    {
        if (zobristKey == entry->key)
        {
            hashedScore = transpositionTabel.ScoreFromTT(entry->score, ply);
            hashedType = entry->type;
            standPat = entry->eval;
        }

        if (!pvNode && transpositionTabel.checkForMoreInformation(hashedType, hashedScore, beta))
        {
            if ((hashedType == EXACT) ||
                (hashedType == UPPER_BOUND && hashedScore <= alpha) ||
                (hashedType == LOWER_BOUND && hashedScore >= beta))
            {
                return hashedScore;
            }
        }
    }

    if (!board.inCheck() && transpositionTabel.checkForMoreInformation(hashedType, hashedScore, standPat))
    {
        standPat = hashedScore;
    }

    if (standPat == NO_VALUE)
    {
        standPat = network.Evaluate((int)board.sideToMove());
    }

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
    Move bestMoveInQs = Move::NULL_MOVE;

    for (Move& move : moveList)
    {
        // Static Exchange Evaluation
        if (!see(board, move, 0))
        {
            continue;
        }
        
        board.makeMove(move);

        int score = -qs(-beta, -alpha, board, ply);

        board.unmakeMove(move);
        //Our current Score is better then the previos bestScore so we update it 
        if (score > bestScore)
        {
            bestScore = score;

            //Score is greater than alpha so we update alpha to the score
            if (score > alpha)
            {
                alpha = score;

                bestMoveInQs = move;
            }

            //Beta cutoff
            if (score >= beta)
            {
                break;
            }
        }
    }

    //Checks for checkmate
    if (board.inCheck() && bestScore == -infinity)
    {
        return -infinity + ply;
    }

    transpositionTabel.storeEvaluation(zobristKey, 0, bestScore >= beta ? LOWER_BOUND : UPPER_BOUND, transpositionTabel.ScoreToTT(bestScore, ply), bestMoveInQs, standPat);

    return bestScore;
}

int Search::getPieceValue(const Board &board, Move &move)
{
    auto moveType = move.typeOf();

    if (moveType == move.CASTLING)
    {
        return 0;
    }

    if (moveType == move.ENPASSANT)
    {
        return SEE_PIECE_VALUES[0];
    }

    int score = SEE_PIECE_VALUES[(int)board.at<PieceType>(move.to())];

    if (moveType == move.PROMOTION)
    {
        score += SEE_PIECE_VALUES[(int)move.promotionType()] - SEE_PIECE_VALUES[0];
    }

    return score;
}


// SEE prunning by Starzix
bool Search::see(const Board &board, Move &move, int cutoff)
{
    int score = getPieceValue(board, move) - cutoff;
    if (score < 0)
    {
        return false;
    }

    PieceType next = (move.typeOf() == move.PROMOTION) ? move.promotionType() : board.at<PieceType>(move.from());
    score -= SEE_PIECE_VALUES[(int)next];
    if (score >= 0)
    {
        return true;
    }

    int from = move.from().index();
    int to = move.to().index();

    Bitboard occupancy = board.occ() ^ (1ULL << from) ^ (1ULL << to);
    Bitboard queens = board.pieces(PieceType::QUEEN);
    Bitboard bishops = queens | board.pieces(PieceType::BISHOP);
    Bitboard rooks = queens | board.pieces(PieceType::ROOK);

    Square square = move.to();

    Bitboard attackers = 0;
    attackers |= rooks & attacks::rook(square, occupancy);
    attackers |= bishops & attacks::bishop(square, occupancy);
    attackers |= board.pieces(PieceType::PAWN, Color::BLACK) & attacks::pawn(Color::WHITE, square);
    attackers |= board.pieces(PieceType::PAWN, Color::WHITE) & attacks::pawn(Color::BLACK, square);
    attackers |= board.pieces(PieceType::KNIGHT) & attacks::knight(square);
    attackers |= board.pieces(PieceType::KING) & attacks::king(square);

    Color us = ~board.sideToMove();
    while (true)
    {
        Bitboard ourAttackers = attackers & board.us(us);
        if (ourAttackers == 0)
        {
            break;
        }

        next = getLeastValuableAttacker(board, occupancy, ourAttackers, us);

        if (next == PieceType::PAWN || next == PieceType::BISHOP || next == PieceType::QUEEN)
        {
            attackers |= attacks::bishop(square, occupancy) & bishops;
        }

        if (next == PieceType::ROOK || next == PieceType::QUEEN)
        {
            attackers |= attacks::rook(square, occupancy) & rooks;
        }

        attackers &= occupancy;
        score = -score - 1 - SEE_PIECE_VALUES[(int)next];
        us = ~us;

        if (score >= 0)
        {
            // If our only attacker is our king, but the opponent still has defenders
            if (next == PieceType::KING && (attackers & board.us(us)).getBits() > 0)
            {
                us = ~us;
            }
            break;
        }
    }

    return board.sideToMove() != us;
}

PieceType Search::getLeastValuableAttacker(const Board &board, Bitboard &occ, Bitboard attackers, Color color)
{
    for (int piece = 0; piece <= 5; piece++)
    {
        Bitboard bitboard = attackers & board.pieces((PieceType)piece, color);
        if (bitboard.getBits() > 0)
        {
            occ ^= (1ULL << bitboard.lsb());
            return (PieceType)piece;
        }
    }

    // If no attacker is found we return an empty piece
    return PieceType::NONE;
}
void Search::iterativeDeepening(Board& board)
{
    start = std::chrono::high_resolution_clock::now();
    timeForMove = getTimeForMove();
    bestMove = Move::NULL_MOVE;
    Move bestMoveThisIteration = Move::NULL_MOVE;
    isNormalSearch = false;
    bool hasFoundMove = false;
    std::uint64_t key = board.zobrist();

    //If there is no time left make a search at depth 1
    if (timeForMove == -20)
    {
        pvs(-32767, 32767, 1, 0, board);
        if (bestMove != Move::NULL_MOVE)
        {
            storeKey(key);
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
            storeKey(key);
            std::cout << "bestmove " << bestMove << std::endl;
            break;
        }

        if (isOver && hasFoundMove)
        {
            storeKey(key);
            std::cout << "bestmove " << bestMoveThisIteration << std::endl;
            shouldStop = true;
            break;
        }

    }
    shouldStop = false;
    isNormalSearch = true;
}
