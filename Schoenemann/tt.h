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

    void storeEvaluation(int depth, int ply, int score, int nodeType, Move move, Board& board);
    int lookUpEvaluation(int depth, int ply, int alpha, int beta, Board& board);
    entry getEntry(Board& board);
    void clear();
    void init(int size);

private:
    entry* entries = nullptr;
    long long ttSize;
};
#endif