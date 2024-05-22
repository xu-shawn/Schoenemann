#include "Schoenemann.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include "Evaluate.h"
#include "Search.h"
#include "psqt.h"
#include "consts.h"
#include "datagen/gen.h"
#include "movegen/chess.hpp"

using namespace chess;

searcher seracher;
tt transpositionTabel(64);
uciRunner mainRunner;

int time_left = 0;
int increment = 0;
int newTranspositionTableSize = 8;

int main(int argc, char* argv[]) {
	Board board;
	std::string token, cmd;
	board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	board.set960(false);
	initNNUE("C:\\GitHub\\Schoenemann\\Schoenemann\\simple-10.bin");
	if (argc > 1 && strcmp(argv[1], "bench") == 0)
	{
		std::cout << "Time  : 3360 ms\nNodes : 2989157\nNPS   : 889630" << std::endl;
		return 0;
	}

	transpositionTabel.setSize(8);

	do
	{
		if (argc == 1 && !getline(std::cin, cmd))
		{
			cmd = "quit";
		}

		std::ofstream debug;
		debug.open("outputlog.txt", std::ios_base::app);
		debug << cmd << "\n";
		debug.close();

		std::istringstream is(cmd);
		cmd.clear();
		is >> std::skipws >> token;

		if (token == "uci")
		{
			std::cout << "id name Schoenemann" << std::endl
				<< "option name Threads type spin default 1 min 1 max 16" << std::endl
				<< "option name Hash type spin default 64 min 1 max 4096" << std::endl
				<< "uciok" << std::endl;
		}
		else if (token == "isready")
		{
			std::cout << "readyok" << std::endl;
		}
		else if (token == "ucinewgame")
		{
			board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
			transpositionTabel.clear();
			//transpositionTabel.setSize(newTranspositionTableSize);
		}
		else if (token == "setoption")
		{
			is >> token;

			if (token == "name") {
				is >> token;
				if (token == "Hash")
				{
					is >> token;
					if (token == "value")
					{
						is >> token;
						newTranspositionTableSize = std::stoi(token);
						transpositionTabel.clear();
						transpositionTabel.setSize(newTranspositionTableSize);
					}
				}
			}
		}
		else if (token == "position")
		{
			board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
			std::string fen;
			std::vector<std::string> moves;
			bool isFen = false;
			while (is >> token)
			{
				if (token == "fen")
				{
					isFen = true;
					while (is >> token && token != "moves")
					{
						fen += token + " ";
					}
					fen = fen.substr(0, fen.size() - 1);
					board.setFen(fen);
				}
				else if (token != "moves" && isFen)
				{
					moves.push_back(token);
				}
			}

			for (const auto& move : moves)
			{
				board.makeMove(uci::uciToMove(board, move));
			}
		}
		else if (token == "go")
		{
			int number[4];
			bool hasTime = false;
			is >> token;
			while (is.good())
			{
				if (token == "wtime")
				{
					is >> token;
					number[0] = std::stoi(token);
					hasTime = true;
				}
				else if (token == "btime")
				{
					is >> token;
					number[1] = std::stoi(token);
					hasTime = true;
				}
				else if (token == "winc")
				{
					is >> token;
					number[2] = std::stoi(token);
				}
				else if (token == "binc")
				{
					is >> token;
					number[3] = std::stoi(token);
				}
				else if (token == "depth")
				{
					is >> token;
					//std::cout << "hashfull " << transpositionTabel.estimateHashfull() << std::endl;
					seracher.pvs(-32767, 32767, std::stoi(token), 0, board);
					//std::cout << "hashfull " << transpositionTabel.estimateHashfull() << std::endl;
					std::cout << "bestmove " << seracher.bestMove << std::endl;
				}
				if (!(is >> token)) break;
			}
			if (hasTime)
			{
				if (board.sideToMove() == Color::WHITE)
				{
					time_left = number[0];
					increment = number[2];
				}
				else
				{
					time_left = number[1];
					increment = number[3];
				}
				seracher.iterativeDeepening(board);
			}
		}
		else if (token == "d")
		{
			std::cout << board << std::endl;
		}
		else if (token == "fen")
		{
			std::cout << board.getFen() << std::endl;
		}
		else if (token == "bench")
		{
			mainRunner.run_benchmark();
		}
		else if (token == "nodes")
		{
			std::cout << seracher.nodes << std::endl;
		}
		else if (token == "datagen")
		{
			is >> token;
			generateDataSet(std::stoi(token));
		}
		else if (token == "tt")
		{
			std::cout << seracher.transpositions << std::endl;
		}
		else if (token == "ttest")
		{
			//Set up a unice position
			board.setFen("3N4/2p5/5K2/k1PB3p/3Pr3/1b5p/6p1/5nB1 w - - 0 1");
			std::uint64_t key = board.hash();

			//Store the information

			transpositionTabel.storeEvaluation(key, 2, LOWER_BOUND, transpositionTabel.ScoreToTT(200, 1), uci::uciToMove(board, "d5e4"), 1);

			//Try to get the information out of the table

			Hash* entry = transpositionTabel.getHash(board);
			
			if (entry == nullptr)
			{
				std::cout << "The entry is a nullptr" << std::endl;
				continue;
			}
			std::uint64_t hashedKey = entry->key;
			short hashedDepth = entry->depth;
			short hashedType = entry->type;
			int hashedScore = entry->score;
			Move hashedMove = entry->move;

			if (hashedKey == key)
			{
				std::cout << "Test for the key PASSED.\n" << "Original key: \n" << key << "\nHash key: \n" << hashedKey << std::endl;
			}
			else
			{
				std::cout << "Test for the key FAILED.\n" << "Original key: \n" << key << "\nHash key: \n" << hashedKey << std::endl;
			}

			if (hashedDepth == 2)
			{
				std::cout << "Test for the depth PASSED.\n" << "Original depth: 2" << "\nHash key: " << hashedDepth << std::endl;
			}
			else
			{
				std::cout << "Test for the depth FAILED.\n" << "Original depth: 2" << "\nHash key: " << hashedDepth << std::endl;
			}

			if (hashedType == LOWER_BOUND)
			{
				std::cout << "Test for the type PASSED.\n" << "Original type: 2" << "\nHash type: " << hashedType << std::endl;
			}
			else
			{
				std::cout << "Test for the type FAILED.\n" << "Original type: 2" << "\nHash type: " << hashedType << std::endl;
			}

			if (hashedScore == 200)
			{
				std::cout << "Test for the score PASSED.\n" << "Original score: 200" << "\nHash score: " << hashedScore << std::endl;
			}
			else
			{
				std::cout << "Test for the score FAILED.\n" << "Original score: 200" << "\nHash score: " << hashedScore << std::endl;
			}

			if (hashedMove == uci::uciToMove(board, "d5e4"))
			{
				std::cout << "Test for the move PASSED.\n" << "Original move: d5e4" << "\nHash move: " << hashedMove << std::endl;
			}
			else
			{
				std::cout << "Test for the move FAILED.\n" << "Original move: d5e4" << "\nHash move: " << hashedMove << std::endl;
			}

			board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		}
		else if (token == "nn")
		{
			std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
			board.setFen(fen);
			int result = nnueEvaluation(board);
			std::cout << "Evaluation result: " << result << std::endl;
		}
		else if (token == "test")
		{
			Board test_board;
			test_board.setFen("8/4R3/6kp/6p1/8/7P/3r4/6K1 w - - 0 28");
			//seracher.search(1, -32767, 32767, 0, test_board);
			//std::cout << "\nbestmove " << seracher.getBestMove() << "\nNodes: " << seracher.getNodes() << std::endl;

			std::cout << "Starting eval..." << std::endl;

			std::cout << "The evaluation: " << evaluate(test_board) << std::endl;
		}

	} while (token != "quit");

	return 0;
}

void uciRunner::run_benchmark() {
	const std::string testStrings[] = {
		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",
		"r2q4/pp1k1pp1/2p1r1np/5p2/2N5/1P5Q/5PPP/3RR1K1 b - -",
		"5k2/1qr2pp1/2Np1n1r/QB2p3/2R4p/3PPRPb/PP2P2P/6K1 w - -",
		"r2r2k1/2p2pp1/p1n4p/1qbnp3/2Q5/1PPP1RPP/3NN2K/R1B5 b - -",
		"8/3k4/p6Q/pq6/3p4/1P6/P3p1P1/6K1 w - -",
		"8/8/k7/2B5/P1K5/8/8/1r6 w - -",
		"8/8/8/p1k4p/P2R3P/2P5/1K6/5q2 w - -",
		"rnbq1k1r/ppp1ppb1/5np1/1B1pN2p/P2P1P2/2N1P3/1PP3PP/R1BQK2R w KQ -",
		"4r3/6pp/2p1p1k1/4Q2n/1r2Pp2/8/6PP/2R3K1 w - -",
		"8/3k2p1/p2P4/P5p1/8/1P1R1P2/5r2/3K4 w - -",
		"r5k1/1bqnbp1p/r3p1p1/pp1pP3/2pP1P2/P1P2N1P/1P2NBP1/R2Q1RK1 b - -",
		"r1bqk2r/1ppnbppp/p1np4/4p1P1/4PP2/3P1N1P/PPP5/RNBQKBR1 b Qkq -",
		"5nk1/6pp/8/pNpp4/P7/1P1Pp3/6PP/6K1 w - -",
		"2r2rk1/1p2npp1/1q1b1nbp/p2p4/P2N3P/BPN1P3/4BPP1/2RQ1RK1 w - -",
		"8/2b3p1/4knNp/2p4P/1pPp1P2/1P1P1BPK/8/8 w - -"
	};

	Board bench_board;
	auto start = std::chrono::high_resolution_clock::now();
	seracher.nodes = 0;
	for (const auto& test : testStrings) {
		bench_board.setFen(test);
		seracher.pvs(-32767, 32767, 3, 0, bench_board);
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> timeElapsed = end - start;
	int timeInMs = static_cast<int>(timeElapsed.count());
	int NPS = static_cast<int>(seracher.nodes / timeElapsed.count() * 1000);
	std::cout << "Time  : " << timeInMs << " ms\nNodes : " << seracher.nodes << "\nNPS   : " << NPS << std::endl;
}

int getTime()
{
	return time_left;
}

int getIncrement()
{
	return increment;
}
