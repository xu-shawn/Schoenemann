#include <iostream>
#include <fstream>
#include <random>

#include "gen.h"
#include "../Search.h"
#include "../movegen/chess.hpp"

using namespace chess;

searcher search;

void generateDataSet(int epochs)
{
	Board board;
	std::string result;
	std::uint64_t genFens = 0;
	//search.hasNodeLimit = true;
	//search.nodeLimit = 5000;
	for (int i = 0; i < epochs; i++)
	{
		//Playing a game
		//At first play 5 random moves
		board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		for (short x = 0; x < 5; x++)
		{
			Movelist moveList; 
			movegen::legalmoves(moveList, board);
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> distr(0, moveList.size() - 1);
			//std::cout << "size " << moveList.size() << " Number " << distr(gen) << std::endl;
			board.makeMove(moveList.at(distr(gen)));
		}

		result = "";
		
		std::string fens[500];

		for (short x = 0; x < 500; x++)
		{
			if (board.isGameOver().first != GameResultReason::NONE) 
			{
				GameResult gameResult = board.isGameOver().second;

				bool isWhiteTurn = board.sideToMove() == Color::WHITE ? true : false;

				if (gameResult == GameResult::WIN && isWhiteTurn)
				{
					result = "[1-0]";
					writeFile(fens, result);
				}
				else if (gameResult == GameResult::LOSE && isWhiteTurn)
				{
					result = "[0-1]";
					writeFile(fens, result);
				}
				else if (gameResult == GameResult::WIN)
				{
					result = "[0-1]";
					writeFile(fens, result);
				}
				else if (gameResult == GameResult::LOSE)
				{
					result = "[1-0]";
					writeFile(fens, result);
				}
				else
				{
					result = "[1/2-1/2]";
					writeFile(fens, result);
				}
				break;
			}
			//search.setNodes(0);
			//search.search(1, -32767, 32767, 0, board);
			board.makeMove(search.getBestMove());
			fens[x] = board.getFen() + "; ";
			genFens++;

		}

		std::cout << "info epoch " << i << " generated fens " << genFens << std::endl;

	}

	//search.hasNodeLimit = false;
}

void writeFile(std::string fens[], std::string gameResult)
{
	std::ofstream writer;
	writer.open("data.epd", std::ios_base::app);

	for (short i = 0; i < fens->size(); i++)
	{
		if (!fens[i].empty())
		{
			writer << fens[i] + gameResult << "\n";
		}
	}
	
	writer.close();
}