#ifndef TT_H
#define TT_H

#include <iostream>
#include "movegen/chess.hpp"

using namespace chess;

class tt{
public:
	long long ttSize;

	~tt() {
		delete[] entries;
	}


	struct entry {
		std::uint64_t key;
		int eval;
		Move move;
		short depth;
		short nodeType;
	};

	const short exact = 0;
	const short lowerBound = 1;
	const short uppperBound = 2;
	const short lookupFaild = -32768;
	
	void storeEvaluation(int depth, int play, int eval, int nodeType, Move move, Board& board);
	int lookUpEvaluation(int depth, int ply, int alpha, int beta, Board& board);
	entry getEntry(Board& board);
	void clear();
	void init(int size);

private:
	entry* entries;
	int size;
};

#endif