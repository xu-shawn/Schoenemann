#include "helper.h"

void transpositionTableTest(Board& board)
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
		return;
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
	board.setFen(STARTPOS);
}

void testCommand()
{

	Board test_board;
	test_board.setFen("8/4R3/6kp/6p1/8/7P/3r4/6K1 w - - 0 28");
}
