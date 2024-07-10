#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <memory>
#include "surge-master/src/position.h"
#include "surge-master/src/tables.h"
#include "surge-master/src/types.h"
#include <cmath>
#include <unordered_map>

// using namespace chess;
// using json = nlohmann::json;

class Engine {
public:
    Board board;
    std::unordered_map<uint64_t, std::pair<float, int>> transpositionTable;
    Engine(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
        : board(fen) {}

    std::vector<Move> getLegalMoves() {
        std::vector<Move> legalMoves;
        Movelist moves;
        movegen::legalmoves(moves, board);
        for (const auto& move : moves) {
            legalMoves.push_back(move);
        }
        return legalMoves;
    }

    std::vector<Move> getOrderedMoves() {
        auto moves = getLegalMoves();
        std::vector<std::pair<float, Move>> orderedMoves;
        for (const auto& move : moves) {
            board.makeMove(move);
            if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
                board.unmakeMove(move);
                return {move};
            } else if (board.inCheck()) {
                orderedMoves.push_back({50, move});
            } else {
                orderedMoves.push_back({evaluate(), move});
            }
            float score = evaluate();
            board.unmakeMove(move);
        }
        std::sort(orderedMoves.begin(), orderedMoves.end(), [](const auto& a, const auto& b) {
            return a.first > b.first;
        });
        std::vector<Move> ordered;
        for (const auto& move : orderedMoves) {
            ordered.push_back(move.second);
        }
        return ordered;
    }

    std::shared_ptr<Engine> getChild(const Move& move) {
        auto newEngine = std::make_shared<Engine>(*this);
        newEngine->makeMove(move);
        return newEngine;
    }

    void makeMove(const Move& move) {
        board.makeMove(move);
        // hash = computeHash();
    }

    void undoMove(const Move& move) {
        board.unmakeMove(move);
        // hash = computeHash();
    }

    float evaluate() {
        // if (storage.find(hash) != storage.end()) {
        //     return storage[hash];
        // }
        // Evaluation logic here
        float score = 0;
        int white_pawns =0, black_pawns = 0;
        int white_pieces = 0, black_pieces = 0;
        int white_bishops = 0, black_bishops = 0;
        float white_eval = 0, black_eval = 0;
        int white_king = 0, black_king = 0;
        for (int i=0; i<64; i++) {
            int piece = board.at(Square(i));
            if (piece != 12) {
                if (piece < 6) {
                    white_pieces++;
                    switch (piece)
                    {
                    case 0:
                        white_eval += 1 + (float) pawns_util[i]/100;
                        white_pawns++;
                        break;
                    case 1:
                        white_eval += 3 + (float) knights_util[i]/100;
                        break;
                    case 2:
                        // white_eval += bishops_util[i];
                        white_eval += 3.25 + (float) bishops_util[i]/100;
                        white_bishops++;
                        break;
                    case 3:
                        // white_eval += rooks_util[i];
                        white_eval += 5 + (float) rooks_util[i]/100;
                        break;
                    case 4:
                        // white_eval += queens_util[i];
                        white_eval += 9 + (float) queens_util[i]/100;
                        break;
                    case 5:
                        white_king = i;
                        break;
                    default:
                        break;
                    }
                } else {
                    black_pieces++;
                    switch (piece)
                    {
                    case 6:
                        black_eval += 1 + (float) pawns_util[63-i]/100;
                        black_pawns++;
                        break;
                    case 7:
                        black_eval += 3 + (float) knights_util[63-i]/100;
                        break;
                    case 8:
                        // black_eval += bishops_util[63-i];
                        black_eval += 3.25 + (float) bishops_util[63-i]/100;
                        black_bishops++;
                        break;
                    case 9:
                        // black_eval += rooks_util[63-i];
                        black_eval += 5 + (float) rooks_util[63-i]/100;
                        break;
                    case 10:
                        // black_eval += queens_util[63-i];
                        black_eval += 9 + (float) queens_util[63-i]/100;
                        break;
                    case 11:
                        black_king = 63-i;
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        if (white_pieces + black_pieces > 8) {
            white_eval += (float) kings_start_util[white_king]/100;
            black_eval += (float) kings_start_util[black_king]/100;
        } else {
            white_eval += (float) kings_end_util[white_king]/100;
            black_eval += (float) kings_end_util[black_king]/100;
        }
        if (white_bishops >= 2) {
            white_eval += 0.5;
        }
        if (black_bishops >= 2) {
            black_eval += 0.5;
        }
        score = white_eval - black_eval;
        // std::cout << score << std::endl;
        return score;
    }

    std::string getBestMove(int maxDepth = 3) {
        return alphaBetaPruning(maxDepth, -INFINITY, INFINITY).second;
        // return iterativeDeepening(maxDepth).second;
    }

private:
    // std::unordered_map<int, float> storage;
    // std::unordered_map<int, std::pair<int, std::string>> transpositionTable;
    
    std::vector<float> pawns_util = {
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
        5.0, 10.0, 10.0, -20.0, -20.0, 10.0, 10.0, 5.0, 
        5.0, -5.0, -10.0, 0.0, 0.0, -10.0, -5.0, 5.0, 
        0.0, 0.0, 0.0, 20.0, 20.0, 0.0, 0.0, 0.0, 
        5.0, 5.0, 10.0, 25.0, 25.0, 10.0, 5.0, 5.0, 
        10.0, 10.0, 20.0, 30.0, 30.0, 20.0, 10.0, 10.0, 
        50.0, 50.0, 50.0, 50.0, 50.0, 50.0, 50.0, 50.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    };
    
    std::vector<float> knights_util = {
        -50.0, -40.0, -30.0, -30.0, -30.0, -30.0, -40.0, -50.0, 
        -40.0, -20.0, 0.0, 5.0, 5.0, 0.0, -20.0, -40.0, 
        -30.0, 5.0, 10.0, 15.0, 15.0, 10.0, 5.0, -30.0, 
        -30.0, 0.0, 15.0, 20.0, 20.0, 15.0, 0.0, -30.0, 
        -30.0, 5.0, 15.0, 20.0, 20.0, 15.0, 5.0, -30.0, 
        -30.0, 0.0, 10.0, 15.0, 15.0, 10.0, 0.0, -30.0, 
        -40.0, -20.0, 0.0, 0.0, 0.0, 0.0, -20.0, -40.0, 
        -50.0, -40.0, -30.0, -30.0, -30.0, -30.0, -40.0, -50.0, 
    };
    
    std::vector<float> bishops_util = {
        -20.0, -10.0, -10.0, -10.0, -10.0, -10.0, -10.0, -20.0, 
        -10.0, 5.0, 0.0, 0.0, 0.0, 0.0, 5.0, -10.0, 
        -10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, -10.0, 
        -10.0, 0.0, 10.0, 10.0, 10.0, 10.0, 0.0, -10.0, 
        -10.0, 5.0, 5.0, 10.0, 10.0, 5.0, 5.0, -10.0, 
        -10.0, 0.0, 5.0, 10.0, 10.0, 5.0, 0.0, -10.0, 
        -10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -10.0, 
        -20.0, -10.0, -10.0, -10.0, -10.0, -10.0, -10.0, -20.0, 
    };
    
    std::vector<float> rooks_util = {
        0.0, 0.0, 0.0, 5.0, 5.0, 0.0, 0.0, 0.0, 
        -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
        -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
        -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
        -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
        -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
        5.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 5.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    };
    
    std::vector<float> queens_util = {
        -20.0, -10.0, -10.0, -5.0, -5.0, -10.0, -10.0, -20.0, 
        -10.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, -10.0, 
        -10.0, 5.0, 5.0, 5.0, 5.0, 5.0, 0.0, -10.0, 
        0.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -5.0, 
        -5.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -5.0, 
        -10.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -10.0, 
        -10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -10.0, 
        -20.0, -10.0, -10.0, -5.0, -5.0, -10.0, -10.0, -20.0, 
    };

    std::vector<float> kings_start_util = {
        20.0, 30.0, 10.0, 0.0, 0.0, 10.0, 30.0, 20.0, 
        20.0, 20.0, -5.0, -5.0, -5.0, -5.0, 20.0, 20.0, 
        -10.0, -20.0, -20.0, -20.0, -20.0, -20.0, -20.0, -10.0, 
        -20.0, -30.0, -30.0, -40.0, -40.0, -30.0, -30.0, -20.0, 
        -30.0, -40.0, -40.0, -50.0, -50.0, -40.0, -40.0, -30.0, 
        -40.0, -50.0, -50.0, -60.0, -60.0, -50.0, -50.0, -40.0, 
        -60.0, -60.0, -60.0, -60.0, -60.0, -60.0, -60.0, -60.0, 
        -80.0, -70.0, -70.0, -70.0, -70.0, -70.0, -70.0, -80.0, 
    };

    std::vector<float> kings_end_util = {
        -50.0, -30.0, -30.0, -30.0, -30.0, -30.0, -30.0, -50.0, 
        -30.0, -25.0, 0.0, 0.0, 0.0, 0.0, -25.0, -30.0, 
        -25.0, -20.0, 20.0, 25.0, 25.0, 20.0, -20.0, -25.0, 
        -20.0, -15.0, 30.0, 40.0, 40.0, 30.0, -15.0, -20.0, 
        -15.0, -10.0, 35.0, 45.0, 45.0, 35.0, -10.0, -15.0, 
        -10.0, -5.0, 20.0, 30.0, 30.0, 20.0, -5.0, -10.0, 
        -5.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -5.0, 
        -20.0, -10.0, -10.0, -10.0, -10.0, -10.0, -10.0, -20.0, 
    };

    std::pair<float, std::string> iterativeDeepening(int maxDepth) {
        std::string bestMove = "";
        float bestScore = -INFINITY;
        for (int depth = 1; depth <= maxDepth; depth++) {
            std::pair<float, std::string> result = alphaBetaPruning(depth, -INFINITY, INFINITY);
            if (result.first > bestScore) {
                bestScore = result.first;
                bestMove = result.second;
            }
        }
        return {bestScore, bestMove};
    }

    std::pair<float, std::string> alphaBetaPruning(int depth, float alpha, float beta) {
        if (depth == 0) {
            return {evaluate(), ""};
        }
        if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
            if(board.sideToMove() == Color("w")) {
                return {-INFINITY, ""};
            } else {
                return {INFINITY, ""};
            }
        }
        auto moves = getLegalMoves();
        std::string bestMove = uci::moveToUci(moves[0]);
        for (const auto& move : moves) {
            board.makeMove(move);
            float score;
            if (transpositionTable.find(board.hash()) != transpositionTable.end()) {
                std::pair<float, int> transposition = transpositionTable[board.hash()];
                if (transposition.second >= depth) {
                    score = transposition.first;
                } else {
                    score = alphaBetaPruning(depth - 1, alpha, beta).first;
                }
            } else {
                score = alphaBetaPruning(depth - 1, alpha, beta).first;
            }
            // float score = alphaBetaPruning(depth - 1, alpha, beta).first;
            board.unmakeMove(move);
            if (board.sideToMove() == Color("w")) {
                if (score > alpha) {
                    alpha = score;
                    bestMove = uci::moveToUci(move);
                }
            } else {
                if (score < beta) {
                    beta = score;
                    bestMove = uci::moveToUci(move);
                }
            }
            if (alpha >= beta) {
                break;
            }
            // if (depth == 5) {
            //     std::cout << uci::moveToUci(move) << " " << score << std::endl;
            // }
        }
        // std::cout << bestMove << " " << alpha << std::endl;
        transpositionTable[board.hash()] = std::make_pair(board.sideToMove() == Color("w") ? alpha : beta, depth);

        return {board.sideToMove() == Color("w") ? alpha : beta, bestMove};
    }

};

int main() {
    Engine engine;
    std::string pgn = "";
    int move_count = 0;
    // while(engine.board.isGameOver().first == GameResultReason::NONE) {
    //     // std::cout << "Best Move: " << engine.getBestMove(5) << std::endl;
    //     Move move = uci::uciToMove(engine.board, engine.getBestMove(7));
    //     if (move_count % 2 == 0) {
    //         pgn += std::to_string(move_count/2 + 1) + ". ";
    //     }
    //     pgn += uci::moveToSan(engine.board, move);
    //     engine.makeMove(move);
    //     // std::cout << engine.board << '\n';
    //     pgn += " ";
    //     move_count++;
    //     std::cout << move_count << '\n';
    //     // std::cout << pgn << std::endl;
    // }
    // std::cout << pgn << std::endl;
    std::cout << "Best Move: " << engine.getBestMove(5) << std::endl;
    // std::cout << engine.board.hash() << std::endl;
    return 0;
}