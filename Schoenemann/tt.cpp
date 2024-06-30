#include <iostream>
#include <cstring>

#include "tt.h"

void tt::storeEvaluation(std::uint64_t key, short depth, short type, int score, Move move, int eval)
{
	std::uint64_t index = key % size;

	//Get the HashNode
	Hash* node = table + index;

	//Store the entry
	node->setEntry(key, depth, type, score, move, eval);
}


Hash *tt::getHash(Board& board)
{
	//Calculates the zobrish key
	uint64_t zobristKey = board.hash();

	//Gets the index based on the zobrist key
	uint64_t index = zobristKey % size;

	//Getting the node by the index
	Hash* node = table + index;

	//Check all two buckets
	if (node->key == zobristKey)
	{
		return node;
	}

	//Returns a nullptr if nothing was found in the hash
	return nullptr;
}
void tt::clear() 
{
	memset(static_cast<void*>(table), 0, size * sizeof(Hash));

}

void tt::init(uint64_t MB) 
{
	uint64_t bytes = MB << 20;
	uint64_t maxSize = bytes / sizeof(Hash);

	size = 1;
	while (size <= maxSize)
	{
		size <<= 1;
	}
		
	size >>= 1;

	table = (Hash *) calloc(size, sizeof(Hash));
	clear();
}

uint64_t tt::getSize() const 
{
	return (2 * size);
}

void tt::setSize(uint64_t MB) 
{
	free(table);
	init(MB);
}

int tt::estimateHashfull() const 
{
	int used = 0;

	return used;
}


tt::tt(uint64_t MB) 
{
	init(MB);
}

tt::~tt() 
{
	free(table);
}
