#pragma once
#include "movegen/chess.hpp"

using namespace chess;

class searcher {
public:
	const short infinity = 32767;
	int countNodes = 0;
	int transpositions = 0;
	Move bestMove = Move::NULL_MOVE;
	bool shouldStop = false;
	bool isNormalSearch = true;
	int timeForMove = 0;
	bool hasNodeLimit = false;
	int nodeLimit = 0;

	int checkQuiescenceSearch(int depth, int alpha, int beta, int ply, Board& board);
	int search(int depth, int alpha, int beta, int ply, Board& board);
	int quiescenceSearch(int depth, int alpha, int beta, int ply, Board& board);
	void iterativeDeepening(Board& board);

	int getNodes()
	{
		return countNodes;
	}

	void setNodes(int newNodes)
	{
		countNodes = newNodes;
	}
	
	Move& getBestMove()
	{
		return bestMove;
	}
	
	int getTranspositions()
	{
		return transpositions;
	}
private:
};
