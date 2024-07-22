#include "Schoenemann.h"
#include "search.h"
#include "nnue.h"
#include "consts.h"
#include "helper.h"
#include "datagen/gen.h"
#include "movegen/chess.hpp"

using namespace chess;

Search seracher;
tt transpositionTabel(8);
uciRunner mainRunner;
psqt bouns;

int timeLeft = 0;
int increment = 0;
int newTranspositionTableSize = 8;

int main(int argc, char* argv[]) {

	//The main board
	Board board;

	//UCI-Command stuff
	std::string token, cmd;

	//Reset the board
	board.setFen(STARTPOS);

	//Disable FRC (Fisher-Random-Chess)
	board.set960(false);
	transpositionTabel.setSize(8);
	if (argc > 1 && strcmp(argv[1], "bench") == 0)
	{
		mainRunner.run_benchmark();
		return 0;
	}

	//Main UCI-Loop
	do
	{
		if (argc == 1 && !getline(std::cin, cmd))
		{
			cmd = "quit";
		}

		/*
		std::ofstream debug;
		debug.open("outputlog.txt", std::ios_base::app);
		debug << cmd << "\n";
		debug.close();
		*/

		std::istringstream is(cmd);
		cmd.clear();
		is >> std::skipws >> token;

		if (token == "uci")
		{
			uciPrint();
		}
		else if (token == "isready")
		{
			std::cout << "readyok" << std::endl;
		}
		else if (token == "ucinewgame")
		{
			//Reset the board
			board.setFen(STARTPOS);

			//Clear the transposition table
			transpositionTabel.clear();
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
			board.setFen(STARTPOS);
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
					seracher.pvs(-32767, 32767, std::stoi(token), 0, board);
					std::cout << "bestmove " << seracher.bestMove << std::endl;
				}
				if (!(is >> token)) break;
			}
			if (hasTime)
			{
				if (board.sideToMove() == Color::WHITE)
				{
					timeLeft = number[0];
					increment = number[2];
				}
				else
				{
					timeLeft = number[1];
					increment = number[3];
				}
				seracher.iterativeDeepening(board);
			}
		}
		else if (token == "d")
		{
			std::cout << board << std::endl;
		}
		else if (token == "bench")
		{
			run_benchmark();
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
		else if (token == "ttest")
		{
			transpositionTableTest(board);
		}
		else if (token == "nn")
		{
			//Set up a unice position
			board.setFen("3N4/2p5/5K2/k1PB3p/3Pr3/1b5p/6p1/5nB1 w - - 0 1");
			std::uint64_t key = board.zobrist();

			//Store the information

			transpositionTabel.storeEvaluation(key, 2, LOWER_BOUND, transpositionTabel.ScoreToTT(200, 1), uci::uciToMove(board, "d5e4"), 1);

			//Try to get the information out of the table

			Hash* entry = transpositionTabel.getHash(key);
			
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
		else if (token == "test")
		{
			testCommand();
		}

	} while (token != "quit");

	return 0;
}

void uciRunner::run_benchmark() {//Setting up the bench Board
	Board benchBoard;

	//Setting up the clock 
	auto start = std::chrono::high_resolution_clock::now();

	//Reseting the nodes
	seracher.nodes = 0;

	//Looping over all bench positions
	for (const auto& test : testStrings) {
		benchBoard.setFen(test);
		seracher.pvs(-infinity, infinity, benchDepth, 0, benchBoard);
	}

	auto end = std::chrono::high_resolution_clock::now();

	//Calculates the total time used
	std::chrono::duration<double, std::milli> timeElapsed = end - start;
	int timeInMs = static_cast<int>(timeElapsed.count());

	//calculates the Nodes per Second
	int NPS = static_cast<int>(seracher.nodes / timeElapsed.count() * 1000);

	//Prints out the final bench 
	std::cout << "Time  : " << timeInMs << " ms\nNodes : " << seracher.nodes << "\nNPS   : " << NPS << std::endl;
}

int getTime()
{
	return timeLeft;
}

int getIncrement()
{
	return increment;
}
