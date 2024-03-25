#include <chrono>
#include <iomanip>

#include "chess.hpp"
using namespace std::chrono;

class ChessPerft {
public:
    virtual void setup(std::string fen) = 0;

    virtual uint64_t perft(int depth) = 0;

    void benchPerft(std::string fen, int depth, uint64_t expected_node_count) {
        setup(fen);

        const auto t1 = high_resolution_clock::now();
        const auto nodes = perft(depth);
        const auto t2 = high_resolution_clock::now();
        const auto ms = duration_cast<milliseconds>(t2 - t1).count();

        std::stringstream ss;
        ss << "depth " << std::left << std::setw(2) << depth
            << " time " << std::setw(5) << ms
            << " nodes " << std::setw(12) << nodes
            << " nps " << std::setw(9) << (nodes * 1000) / (ms + 1)
            << " fen " << std::setw(87) << fen;
        std::cout << ss.str() << std::endl;
    }
};

class DisservinChess : public ChessPerft {
public:
    void setup(std::string fen) override { board_.setFen(fen); }

    uint64_t perft(int depth) {
        chess::Movelist moves;
        chess::movegen::legalmoves(moves, board_);

        if (depth == 1) {
            return moves.size();
        }

        uint64_t nodes = 0;

        for (const auto& move : moves) {
            board_.makeMove(move);
            nodes += perft(depth - 1);
            board_.unmakeMove(move);
        }

        return nodes;
    }

private:
    chess::Board board_;
};



struct Test {
    std::string fen;
    uint64_t expected_node_count;
    int depth;
};

void perft() {
    const Test test_positions[] = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 119060324, 6},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 193690690, 5},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 11030083, 6},
        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 15833292, 5},
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 89941194, 5},
        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1"
         "1",
         164075551, 5} };

    DisservinChess perft;

    for (const auto& test : test_positions) {
        perft.benchPerft(test.fen, test.depth, test.expected_node_count);
    }
}