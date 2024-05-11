#include <iostream>

#include "tt.h"

void tt::storeEvaluation(Board& board, int score, Move move, int eval, int depth, uint8_t nodeType)
{

	//Get the zobrist key for the position
	std::uint64_t zobristKey = board.zobrist();

	//Get the index
	std::uint64_t index = zobristKey & (size - 1);

	//Get the HashNode
	HashNode *node = table + index;

	//Replacment if the first slot hase the same zobrist key
	//Because a more up to date position should always been choosen
	if (node->slot1.key == zobristKey)
	{
		node->slot1.setEntry(board, score, move, eval, depth, nodeType, age);
	}

	//Replacment if the second slot hase the same zobrist key
	else if (node->slot2.key == zobristKey)
	{
		node->slot2.setEntry(board, score, move, eval, depth, nodeType, age);
	}

	//Add a position to the hash
	//Also replaces the node
	else
	{
		HashEntry* replace = &(node->slot1);

		//Calculates the different scorese for the tow bucket system
		//Uses the ageing method https://www.chessprogramming.org/Transposition_Table (Aging)
		int score1 = 16 * ((int)((uint8_t)(age - (node->slot1.ageNodeType >> 2)))) + depth - node->slot1.depth;
		int score2 = 16 * ((int)((uint8_t)(age - (node->slot2.ageNodeType >> 2)))) + depth - node->slot2.depth;

		if (score1 < score2)
		{
			replace = &(node->slot2);
		}

		if (score1 >= -2 || score2 >= -2)
		{
			replace->setEntry(board, score, move, eval, depth, nodeType, age);
		}
	}
}

HashEntry *tt::getHash(Board& board)
{
	//Calculates the zobrish key
	uint64_t zobristKey = board.hash();

	//Gets the index based on the zobrist key
	uint64_t index = zobristKey & (size - 1);

	//Getting the node by the index
	HashNode* node = table + index;

	//Check all two buckets
	if (node->slot1.key == zobristKey)
	{
		return &(node->slot1);
	}
	else if (node->slot2.key == zobristKey)
	{
		return &(node->slot2);
	}

	//Returns a nullptr if nothing was found in the hash
	return nullptr;
}

void tt::incrementAge() 
{
	//Increments the age by on
	age++;
}

void tt::clear() 
{
	std::memset(static_cast<void*>(table), 0, size * sizeof(HashNode));
	age = 0;
}

void tt::init(uint64_t MB) 
{
	uint64_t bytes = MB << 20;
	uint64_t maxSize = bytes / sizeof(HashNode);

	size = 1;
	while (size <= maxSize)
	{
		size <<= 1;
	}
		
	size >>= 1;

	table = (HashNode *) calloc(size, sizeof(HashNode));
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

	for (int i = 0; i < 500; i++) 
	{
		used += ((table + i)->slot1.ageNodeType >> 2) == age;
		used += ((table + i)->slot2.ageNodeType >> 2) == age;
	}
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
