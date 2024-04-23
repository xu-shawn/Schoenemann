#pragma once
#include "movegen/chess.hpp"

using namespace chess;

class searcher {
public:
	const short infinity = 32767;
	int count_nodes = 0;
	int transpositions = 0;
	Move bestMove = Move::NULL_MOVE;
	bool shouldStop = false;
	bool isNormalSearch = true;
	int timeForMove = 0;

	int getNodes();
	void setNodes(int newNodes);
	int search(int depth, int alpha, int beta, int ply, Board& board);
	Move& getBestMove();
	int quiescenceSearch(int depth, int alpha, int beta, int ply, Board& board);
	void iterativeDeepening(Board& board);
	int getTranspositions();
	int checkQuiescenceSearch(int depth, int alpha, int beta, int ply, Board& board);
private:
};
