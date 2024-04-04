#include <iostream>
#include "movegen/chess.hpp"

using namespace chess;

class tt{
public:
	long long ttSize;
	tt(int size) {
		long long bytes = (long long)size << 20;
		long long maxSize = bytes / sizeof(entry);
		entries = new entry[maxSize];
		ttSize = maxSize;
		this->size = maxSize;
	}


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
	
	void storeEvaluation(int depth, int play, int eval, int nodeType, Move move, Board& board);
	int lookUpEvaluation(int depth, int ply, int alpha, int beta, Board& board);
	Move getStoredMove(Board& board);
	void clear(tt obj);

private:
	entry* entries;
	int size;
};