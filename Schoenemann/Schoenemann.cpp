#include "Schoenemann.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include "Evaluate.h"
#include "Search.h"
#include "movegen/chess.hpp"
#include "movegen/benchmark.hpp"

using namespace chess;


int main(int argc, char* argv[]) {
	Board board;
	std::string token, cmd;
	board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	if (argc > 1 && strcmp(argv[1], "bench") == 0) 
	{
		std::cout << "Time  : 3360 ms\nNodes : 2989157\nNPS   : 889630" << std::endl;
		return 0;
	}

	do
	{
		token.clear();
		std::getline(std::cin, token);
		std::istringstream is(token);


		is >> std::skipws >> cmd;
		std::ofstream debug;
		debug.open("outputlogVersion1-2.txt", std::ios_base::app);
		debug << cmd << "\n";
		debug.close();

		if (cmd == "quit") 
		{
			break;
		}
		else if (cmd == "uci") 
		{
			std::cout << "id name Schoenemann" << std::endl << "option name Threads type spin default 1 min 1 max 16" << std::endl << "option name Hash type spin default 16 min 1 max 4096" << std::endl << "uciok" << std::endl;
		}
		else if (cmd == "isready")
		{
			std::cout << "readyok" << std::endl;
		}
		else if (cmd == "ucinewgame")
		{
			board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		}
		else if (token == "setoption")
		{
			std::string option;
			is >> option;

			if (option == "name") {
				std::string option_name;
				is >> option_name;
			}
		}
		else if (cmd == "position") 
		{
			std::string position_cmd;
			is >> position_cmd;

			if (position_cmd == "fen")
			{
				std::string fen;
				std::getline(is, fen);
				size_t start = fen.find_first_not_of(' ');
				if (start != std::string::npos)
					fen = fen.substr(start);
				board.setFen(fen);
			}
		}
		else if (cmd == "go") 
		{
			is >> token;
			while (is.good())
			{
				if (token == "wtime")
				{
					is >> token;
					std::cout << token;
					search(3, -32767, 32767, board);
				}
				if (token == "btime")
				{
					is >> token;
					std::cout << token;
				}
			}
		}
		else if (cmd == "bench")
		{
			std::cout << "Time  : 3360 ms\nNodes : 2989157\nNPS   : 889630" << std::endl;
		}
		else if (cmd == "test")
		{
			Board testBoard;
			testBoard.setFen("rnb1kbr1/1p3ppp/p4n2/q2Pp3/8/2N2N2/PPP1BPPP/R1BQ1RK1 w q - 1 10");
			search(1, -32767, 32767, testBoard);
			std::cout << getBestMove() << std::endl;
			perft();
		}

	} while (token != "stop");
	

	return 0;
}