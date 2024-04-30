#ifndef TT_H
#define TT_H

#include <iostream>
#include "movegen/chess.hpp"

using namespace chess;

class tt{
public:
    struct entry {
        std::uint64_t key;
        int score;
        Move move;
        short depth;
        short nodeType;
    };

    const short exact = 0;
    const short lowerBound = 1;
    const short uppperBound = 2;
    const short lookupFaild = -1;
    const short infinity = 32767;

    void storeEvaluation(int depth, int ply, int score, int nodeType, Move move, Board& board);
    int lookUpEvaluation(int depth, int ply, int alpha, int beta, Board& board);
    int correctMateScoreForStorage(int ply, int score, bool isMate);
    int correctRetrievedMateScore(int ply, int score, bool isMate);
    entry getEntry(Board& board);
    void clear();
    void init(int size);

private:
    entry* entries;
    long long ttSize;
    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    bool isMateScore(int score)
    {
        if (score == -infinity)
        {
            return score;
        }
        const int maxMateDepth = 1000;
        return std::abs(score) > infinity - maxMateDepth;
    }
};
#endif