#ifndef TT_H
#define TT_H

#include <iostream>
#include "movegen/chess.hpp"

using namespace chess;

constexpr uint8_t PV_NODE = 0;
constexpr uint8_t CUT_NODE = 1;
constexpr uint8_t ALL_NODE = 2;
constexpr uint8_t NO_NODE_INFO = 3;
constexpr int MAX_PLY_MATE_SCORE = 32500;

struct HashEntry {
    std::uint64_t key;
    int score;
    Move move;
    int eval;
    short depth;
    uint8_t ageNodeType;

    void setEntry(Board& board, int _score, Move _move, int _eval, int _depth, uint8_t _nodeType, uint8_t _age) {
        key = board.hash();
        score = (int16_t)_score;
        move = _move;
        eval = (int16_t)_eval;
        depth = (int8_t)_depth;
        ageNodeType = (_age << 2) | _nodeType;
    }
};

// For each side we have a a HashEntry
//So we are using a two bucket system
class HashNode {
public:
    HashEntry slot1;
    HashEntry slot2;
};

class tt {
public:
    tt(uint64_t MB);
    tt(const tt& other) = delete;
    tt& operator=(const tt& other) = delete;
    ~tt();

    void storeEvaluation(Board& board, int score, Move move, int eval, int depth, uint8_t nodeType);
    HashEntry* getHash(Board& board);

    uint64_t getSize() const;
    void setSize(uint64_t MB);

    void incrementAge();

    void clear();
    int estimateHashfull() const;

    int adjustHashScore(int score, int plies) {
        if (score >= MAX_PLY_MATE_SCORE)
            return score + plies;
        if (score <= -MAX_PLY_MATE_SCORE)
            return score - plies;
        return score;
    }

private:
    HashNode* table;
    uint64_t size;
    uint8_t age;

    void init(uint64_t MB);
};

#endif TT_H