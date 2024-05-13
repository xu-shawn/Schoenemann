#ifndef TT_H
#define TT_H

#include <iostream>
#include "movegen/chess.hpp"

using namespace chess;

const short EXACT = 0;
const short ALPHA = 1;
const short BETA = 2;
const int MATE_SCORE = 32766;
const int MAX_PLY_MATE_SCORE = 32500;

struct Hash {
    std::uint64_t key;
    short depth;
    short type; //Ether EXACT, ALPHA or BETA
    int score;
    Move move;

    void setEntry(std::uint64_t _key, short _depth, short _type, int _score, Move _move)
    {
        key = _key;
        depth = _depth;
        type = _type;
        score = _score;
        move = _move;
    }
};

class tt {
public:
    tt(uint64_t MB);
    tt(const tt& other) = delete;
    tt& operator=(const tt& other) = delete;
    ~tt();

    void storeEvaluation(std::uint64_t key, short depth, short type, int score, Move move);
    Hash* getHash(Board& board);
    uint64_t getSize() const;
    void setSize(uint64_t MB);

    void clear();
    int estimateHashfull() const;

    int adjustHashScore(int score, int plies) 
    {
        if (score >= MAX_PLY_MATE_SCORE)
        {
            return score + plies;
        }
        if (score <= -MAX_PLY_MATE_SCORE)
        {
            return score - plies;
        }
        return score;
    }

    int scoreMate(bool isInCheck, int ply)
    {
        if (isInCheck)
        {
            return (-MATE_SCORE + ply);
        }
        else
        {
            return 0;
        }
    }

private:
    uint64_t size;
    Hash *table;

    void init(uint64_t MB);
};

#endif TT_H