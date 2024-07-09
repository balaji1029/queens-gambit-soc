#include <iostream>
#include <vector>
#include <unordered_map>
#include "chess-library-master/include/chess.hpp"
#include <cmath>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <fstream>
#include <sstream>

using namespace chess;
// using json = nlohmann::json;

class Engine {
public:
    Board board;
    std::unordered_map<uint64_t, std::pair<float, int>> transpositionTable;
    Engine(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
        : board(fen) {
            load_transposition_table();
        }

    void setFen(const std::string& fen) {
        board = Board(fen);
    }

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
        // for (const auto& move : ordered) {
        //     std::cout << move << std::endl;
        // }
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
        // float score = 0;
        // int white_pawns = 0, black_pawns = 0;
        // int white_pieces = 0, black_pieces = 0;
        // int white_bishops = 0, black_bishops = 0;
        // float white_eval = 0, black_eval = 0;
        // int white_king = 0, black_king = 0;
        // for (int i=0; i<64; i++) {
        //     int piece = board.at(Square(i));
        //     if (piece != 12) {
        //         if (piece < 6) {
        //             white_pieces++;
        //             switch (piece)
        //             {
        //             case 0:
        //                 white_eval += 1 + (float) pawns_util[i]/100;
        //                 white_pawns++;
        //                 break;
        //             case 1:
        //                 white_eval += 3 + (float) knights_util[i]/100;
        //                 break;
        //             case 2:
        //                 // white_eval += bishops_util[i];
        //                 white_eval += 3.25 + (float) bishops_util[i]/100;
        //                 white_bishops++;
        //                 break;
        //             case 3:
        //                 // white_eval += rooks_util[i];
        //                 white_eval += 5 + (float) rooks_util[i]/100;
        //                 break;
        //             case 4:
        //                 // white_eval += queens_util[i];
        //                 white_eval += 9 + (float) queens_util[i]/100;
        //                 break;
        //             case 5:
        //                 white_king = i;
        //                 break;
        //             default:
        //                 break;
        //             }
        //         } else {
        //             black_pieces++;
        //             switch (piece)
        //             {
        //             case 6:
        //                 black_eval += 1 + (float) pawns_util[63-i]/100;
        //                 black_pawns++;
        //                 break;
        //             case 7:
        //                 black_eval += 3 + (float) knights_util[63-i]/100;
        //                 break;
        //             case 8:
        //                 // black_eval += bishops_util[63-i];
        //                 black_eval += 3.25 + (float) bishops_util[63-i]/100;
        //                 black_bishops++;
        //                 break;
        //             case 9:
        //                 // black_eval += rooks_util[63-i];
        //                 black_eval += 5 + (float) rooks_util[63-i]/100;
        //                 break;
        //             case 10:
        //                 // black_eval += queens_util[63-i];
        //                 black_eval += 9 + (float) queens_util[63-i]/100;
        //                 break;
        //             case 11:
        //                 black_king = 63-i;
        //                 break;
        //             default:
        //                 break;
        //             }
        //         }
        //     }
        // }
        // if (white_pieces + black_pieces > 8) {
        //     white_eval += (float) kings_start_util[white_king]/100;
        //     black_eval += (float) kings_start_util[black_king]/100;
        // } else {
        //     white_eval += (float) kings_end_util[white_king]/100;
        //     black_eval += (float) kings_end_util[black_king]/100;
        // }
        // if (white_bishops >= 2) {
        //     white_eval += 0.5;
        // }
        // if (black_bishops >= 2) {
        //     black_eval += 0.5;
        // }
        // score = white_eval - black_eval;
        // // std::cout << score << std::endl;
        // return score;

        // float score = 0;
        // int white_pawns = 0, black_pawns = 0;
        // int white_pieces = 0, black_pieces = 0;
        // int white_bishops = 0, black_bishops = 0;
        // float white_eval = 0, black_eval = 0;
        // int white_king = 0, black_king = 0;

        // // Precompute piece values
        // const float piece_values[6] = {1.0, 3.0, 3.25, 5.0, 9.0, 0.0}; // King value is 0 because it doesn't contribute directly

        // for (int i = 0; i < 64; ++i) {
        //     int piece = board.at(Square(i));
        //     if (piece != 12) {
        //         int mirrored_i = 63 - i;
        //         if (piece < 6) { // White pieces
        //             white_pieces++;
        //             white_eval += piece_values[piece] + (float) piece_util[piece][i] / 100.0;
        //             if (piece == 0) white_pawns++;
        //             if (piece == 2) white_bishops++;
        //             if (piece == 5) white_king = i;
        //         } else { // Black pieces
        //             black_pieces++;
        //             int black_piece = piece - 6;
        //             black_eval += piece_values[black_piece] + (float) piece_util[black_piece][mirrored_i] / 100.0;
        //             if (black_piece == 0) black_pawns++;
        //             if (black_piece == 2) black_bishops++;
        //             if (black_piece == 5) black_king = mirrored_i;
        //         }
        //     }
        // }

        // if (white_pieces + black_pieces > 8) {
        //     white_eval += (float)kings_start_util[white_king] / 100;
        //     black_eval += (float)kings_start_util[black_king] / 100;
        // } else {
        //     white_eval += (float)kings_end_util[white_king] / 100;
        //     black_eval += (float)kings_end_util[black_king] / 100;
        // }

        // if (white_bishops >= 2) white_eval += 0.5;
        // if (black_bishops >= 2) black_eval += 0.5;

        // score = white_eval - black_eval;
        // return score;

        float score = 0;
        int white_pawns = 0, black_pawns = 0;
        int white_pieces = 0, black_pieces = 0;
        int white_bishops = 0, black_bishops = 0;
        float white_eval = 0, black_eval = 0;
        int white_king = 0, black_king = 0;

        const float piece_values[6] = {1.0, 3.0, 3.25, 5.0, 9.0, 0.0}; // Values for pawn, knight, bishop, rook, queen, king

        const std::array<float, 64> pawns_util = {
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            5.0, 10.0, 10.0, -20.0, -20.0, 10.0, 10.0, 5.0,
            5.0, -5.0, -10.0, 0.0, 0.0, -10.0, -5.0, 5.0,
            0.0, 0.0, 0.0, 20.0, 20.0, 0.0, 0.0, 0.0,
            5.0, 5.0, 10.0, 25.0, 25.0, 10.0, 5.0, 5.0,
            10.0, 10.0, 20.0, 30.0, 30.0, 20.0, 10.0, 10.0,
            50.0, 50.0, 50.0, 50.0, 50.0, 50.0, 50.0, 50.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        };
    
        const std::array<float, 64> knights_util = {
            -50.0, -40.0, -30.0, -30.0, -30.0, -30.0, -40.0, -50.0, 
            -40.0, -20.0, 0.0, 5.0, 5.0, 0.0, -20.0, -40.0, 
            -30.0, 5.0, 10.0, 15.0, 15.0, 10.0, 5.0, -30.0, 
            -30.0, 0.0, 15.0, 20.0, 20.0, 15.0, 0.0, -30.0, 
            -30.0, 5.0, 15.0, 20.0, 20.0, 15.0, 5.0, -30.0, 
            -30.0, 0.0, 10.0, 15.0, 15.0, 10.0, 0.0, -30.0, 
            -40.0, -20.0, 0.0, 0.0, 0.0, 0.0, -20.0, -40.0, 
            -50.0, -40.0, -30.0, -30.0, -30.0, -30.0, -40.0, -50.0, 
        };
        
        const std::array<float, 64> bishops_util = {
            -20.0, -10.0, -10.0, -10.0, -10.0, -10.0, -10.0, -20.0, 
            -10.0, 5.0, 0.0, 0.0, 0.0, 0.0, 5.0, -10.0, 
            -10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, -10.0, 
            -10.0, 0.0, 10.0, 10.0, 10.0, 10.0, 0.0, -10.0, 
            -10.0, 5.0, 5.0, 10.0, 10.0, 5.0, 5.0, -10.0, 
            -10.0, 0.0, 5.0, 10.0, 10.0, 5.0, 0.0, -10.0, 
            -10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -10.0, 
            -20.0, -10.0, -10.0, -10.0, -10.0, -10.0, -10.0, -20.0, 
        };
        
        const std::array<float, 64> rooks_util = {
            0.0, 0.0, 0.0, 5.0, 5.0, 0.0, 0.0, 0.0, 
            -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
            -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
            -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
            -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
            -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
            5.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 5.0, 
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
        };
    
        const std::array<float, 64> queens_util = {
            -20.0, -10.0, -10.0, -5.0, -5.0, -10.0, -10.0, -20.0, 
            -10.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, -10.0, 
            -10.0, 5.0, 5.0, 5.0, 5.0, 5.0, 0.0, -10.0, 
            0.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -5.0, 
            -5.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -5.0, 
            -10.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -10.0, 
            -10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -10.0, 
            -20.0, -10.0, -10.0, -5.0, -5.0, -10.0, -10.0, -20.0, 
        };

        const std::array<float, 64> kings_start_util = {
            20.0, 30.0, 10.0, 0.0, 0.0, 10.0, 30.0, 20.0, 
            20.0, 20.0, -5.0, -5.0, -5.0, -5.0, 20.0, 20.0, 
            -10.0, -20.0, -20.0, -20.0, -20.0, -20.0, -20.0, -10.0, 
            -20.0, -30.0, -30.0, -40.0, -40.0, -30.0, -30.0, -20.0, 
            -30.0, -40.0, -40.0, -50.0, -50.0, -40.0, -40.0, -30.0, 
            -40.0, -50.0, -50.0, -60.0, -60.0, -50.0, -50.0, -40.0, 
            -60.0, -60.0, -60.0, -60.0, -60.0, -60.0, -60.0, -60.0, 
            -80.0, -70.0, -70.0, -70.0, -70.0, -70.0, -70.0, -80.0, 
        };

        const std::array<float, 64> kings_end_util = {
            -50.0, -30.0, -30.0, -30.0, -30.0, -30.0, -30.0, -50.0, 
            -30.0, -25.0, 0.0, 0.0, 0.0, 0.0, -25.0, -30.0, 
            -25.0, -20.0, 20.0, 25.0, 25.0, 20.0, -20.0, -25.0, 
            -20.0, -15.0, 30.0, 40.0, 40.0, 30.0, -15.0, -20.0, 
            -15.0, -10.0, 35.0, 45.0, 45.0, 35.0, -10.0, -15.0, 
            -10.0, -5.0, 20.0, 30.0, 30.0, 20.0, -5.0, -10.0, 
            -5.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -5.0, 
            -20.0, -10.0, -10.0, -10.0, -10.0, -10.0, -10.0, -20.0, 
        };

        const std::array<const std::array<float, 64>, 6> piece_util = {pawns_util, knights_util, bishops_util, rooks_util, queens_util, {}};

        for (int i = 0; i < 64; ++i) {
            int piece = board.at(i);
            if (piece != 12) {
                int mirrored_i = 63 - i;
                if (piece < 6) { // White pieces
                    white_pieces++;
                    white_eval += piece_values[piece] + piece_util[piece][i] / 100.0f;
                    if (piece == 0) white_pawns++;
                    if (piece == 2) white_bishops++;
                    if (piece == 5) white_king = i;
                } else { // Black pieces
                    black_pieces++;
                    int black_piece = piece - 6;
                    black_eval += piece_values[black_piece] + piece_util[black_piece][mirrored_i] / 100.0f;
                    if (black_piece == 0) black_pawns++;
                    if (black_piece == 2) black_bishops++;
                    if (black_piece == 5) black_king = mirrored_i;
                }
            }
        }

        const float* king_util = (white_pieces + black_pieces > 8) ? kings_start_util.data() : kings_end_util.data();

        white_eval += king_util[white_king] / 100.0f;
        black_eval += king_util[black_king] / 100.0f;

        if (white_bishops >= 2) white_eval += 0.5f;
        if (black_bishops >= 2) black_eval += 0.5f;

        score = white_eval - black_eval;
        return score;
    }

    // std::string getAlphaMove(int depth) {
    //     return alphaBetaPruning(depth, -100000000, 100000000).second;
    // }

    // std::string getBestMove(int maxDepth = 5) {
        // alphaBetaPruning(maxDepth, -INFINITY, INFINITY);
        // std::cout << "Hello" << std::endl;
        // return alphaBetaPruning(maxDepth, -INFINITY, INFINITY).second;
    //     auto start = std::chrono::high_resolution_clock::now();
    //     if (board.sideToMove() == Color("b")) {
    //         float bestScore = 100000000;
    //         std::string bestMove = "";
    //         for (int depth = 1; depth <= maxDepth; depth++) {
    //             // std::cout << "Depth: " << depth << std::endl;
    //             std::pair<float, std::string> result = alphaBetaPruning(depth, -100000000, 100000000);
    //             auto end = std::chrono::high_resolution_clock::now();
    //             double duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //             // std::cout << duration << std::endl;
    //             if (duration < 1500 && depth == maxDepth) {
    //                 maxDepth++;
    //             }
    //             // std::cout << "Depth: " << depth << std::endl;
    //             // std::cout << "Best Score: " << result.first << (bestScore <= 10000000000) << std::endl;
    //             if (result.first <= bestScore) {
    //                 bestScore = result.first;
    //                 bestMove = result.second;
    //             }
    //             if (bestScore <= -100000000 + 130000) {
    //                 // std::cout << "Depth: " << depth << std::endl;
    //                 break;
    //             }
    //         }
    //         // std::cout << "Depth: " << maxDepth << std::endl;
    //         return bestMove;
    //     }
    //     float bestScore = -100000000;   
    //     std::string bestMove = "";
    //     for (int depth = 1; depth <= maxDepth; depth++) {
    //         // std::cout << "Depth: " << depth << std::endl;
    //         std::pair<float, std::string> result = alphaBetaPruning(depth, -100000000, 100000000);
    //         auto end = std::chrono::high_resolution_clock::now();
    //         double duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //         // std::cout << duration << std::endl;
    //         if (duration < 1500 && depth == maxDepth) {
    //             maxDepth++;
    //         }
    //         // std::cout << "Depth: " << depth << std::endl;
    //         // std::cout << "Best Score: " << result.first << std::endl;
    //         if (result.first >= bestScore) {
    //             bestScore = result.first;
    //             bestMove = result.second;
    //         }
    //         if (bestScore >= 100000000 - 130000) {
    //             // std::cout << "Depth: " << depth << std::endl;
    //             break;
    //         }
    //     }
    //     // std::cout << "Depth: " << maxDepth << std::endl;
    //     return bestMove;
    // }

    std::pair<std::string, int> getBetaMove() {
        auto start = std::chrono::high_resolution_clock::now();
        int depth = 1;
        if (board.sideToMove() == Color("w")) {
            float bestScore = -100000000;
            std::string bestMove = "";
            double duration = 0;
            while (duration <= 10000) {
                // std::cout << "Depth: " << depth << std::endl;
                auto start_ = std::chrono::high_resolution_clock::now();
                bool flag = false;
                int nodes = 0;
                std::pair<float, std::string> result = alphaBetaPruning_flag(depth, -100000000, 100000000, start, flag, nodes, true);
                if (flag) {
                    break;
                }
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                // std::cout << result.first << " " << bestScore << std::endl;
                // std::cout << duration << std::endl;
                double duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
                // std::cout << "Depth: " << depth << " " << duration_ << " " << result.first << " " << result.second << std::endl;

                bestScore = result.first;
                bestMove = result.second;
                if (bestScore >= 100000000 - 10000) {
                    // std::cout << "Depth: " << depth << std::endl;
                    break;
                }
                if (duration > 10000) {
                    break;
                }
                std::cout << "info depth " << depth 
                << " score cp " << static_cast<int>(bestScore * 100) 
                << " time " << static_cast<int>(duration) 
                << " nodes " << nodes << std::endl;
                depth++;
            }
            // std::cout << "Depth: " << depth << std::endl;
            return {bestMove, depth};
        }
        float bestScore = 100000000;
        std::string bestMove = "";
        double duration = 0;
        while (duration <= 10000) {
            // std::cout << "Depth: " << depth << std::endl;
            auto start_ = std::chrono::high_resolution_clock::now();
            bool flag = false;
            int nodes = 0;
            std::pair<float, std::string> result = alphaBetaPruning_flag(depth, -100000000, 100000000, start, flag, nodes, true);
            if (flag) {
                break;
            }
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            double duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
            // std::cout << "Depth: " << depth << " " << duration_ << " " << result.first << " " << result.second << std::endl;
            bestScore = result.first;
            bestMove = result.second;
            if (bestScore <= -100000000 + 10000) {
                // std::cout << "Depth: " << depth << std::endl;
                break;
            }
            if (duration > 10000) {
                break;
            }
            std::cout << "info depth " << depth 
            << " score cp " << static_cast<int>(bestScore * 100) 
            << " time " << static_cast<int>(duration) 
            << " nodes " << nodes << std::endl;
            depth++;
        }
        // std::cout << "Depth: " << depth << std::endl;
        return {bestMove, depth};
    }

    float getEval() {
        return transpositionTable[board.hash()].first;
    }

    std::string getBestMove() {
        return alphaBetaPruning(5, -100000000, 100000000).second;
    }

private:

    void load_transposition_table() {
        std::ifstream file("transposition_table.txt");
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string key;
                float value1;
                int value2;
                if (iss >> key >> value1 >> value2) {
                    uint64_t hash = std::stoull(key);
                    transpositionTable[hash] = {value1, value2};
                }
            }
            file.close();
        }
    }
    
    // std::vector<float> pawns_util = {
    //     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    //     5.0, 10.0, 10.0, -20.0, -20.0, 10.0, 10.0, 5.0, 
    //     5.0, -5.0, -10.0, 0.0, 0.0, -10.0, -5.0, 5.0, 
    //     0.0, 0.0, 0.0, 20.0, 20.0, 0.0, 0.0, 0.0, 
    //     5.0, 5.0, 10.0, 25.0, 25.0, 10.0, 5.0, 5.0, 
    //     10.0, 10.0, 20.0, 30.0, 30.0, 20.0, 10.0, 10.0, 
    //     50.0, 50.0, 50.0, 50.0, 50.0, 50.0, 50.0, 50.0, 
    //     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    // };
    
    // std::vector<float> knights_util = {
    //     -50.0, -40.0, -30.0, -30.0, -30.0, -30.0, -40.0, -50.0, 
    //     -40.0, -20.0, 0.0, 5.0, 5.0, 0.0, -20.0, -40.0, 
    //     -30.0, 5.0, 10.0, 15.0, 15.0, 10.0, 5.0, -30.0, 
    //     -30.0, 0.0, 15.0, 20.0, 20.0, 15.0, 0.0, -30.0, 
    //     -30.0, 5.0, 15.0, 20.0, 20.0, 15.0, 5.0, -30.0, 
    //     -30.0, 0.0, 10.0, 15.0, 15.0, 10.0, 0.0, -30.0, 
    //     -40.0, -20.0, 0.0, 0.0, 0.0, 0.0, -20.0, -40.0, 
    //     -50.0, -40.0, -30.0, -30.0, -30.0, -30.0, -40.0, -50.0, 
    // };
    
    // std::vector<float> bishops_util = {
    //     -20.0, -10.0, -10.0, -10.0, -10.0, -10.0, -10.0, -20.0, 
    //     -10.0, 5.0, 0.0, 0.0, 0.0, 0.0, 5.0, -10.0, 
    //     -10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, -10.0, 
    //     -10.0, 0.0, 10.0, 10.0, 10.0, 10.0, 0.0, -10.0, 
    //     -10.0, 5.0, 5.0, 10.0, 10.0, 5.0, 5.0, -10.0, 
    //     -10.0, 0.0, 5.0, 10.0, 10.0, 5.0, 0.0, -10.0, 
    //     -10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -10.0, 
    //     -20.0, -10.0, -10.0, -10.0, -10.0, -10.0, -10.0, -20.0, 
    // };
    
    // std::vector<float> rooks_util = {
    //     0.0, 0.0, 0.0, 5.0, 5.0, 0.0, 0.0, 0.0, 
    //     -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
    //     -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
    //     -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
    //     -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
    //     -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -5.0, 
    //     5.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 5.0, 
    //     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    // };
    
    // std::vector<float> queens_util = {
    //     -20.0, -10.0, -10.0, -5.0, -5.0, -10.0, -10.0, -20.0, 
    //     -10.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, -10.0, 
    //     -10.0, 5.0, 5.0, 5.0, 5.0, 5.0, 0.0, -10.0, 
    //     0.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -5.0, 
    //     -5.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -5.0, 
    //     -10.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -10.0, 
    //     -10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -10.0, 
    //     -20.0, -10.0, -10.0, -5.0, -5.0, -10.0, -10.0, -20.0, 
    // };

    // std::vector<float> kings_start_util = {
    //     20.0, 30.0, 10.0, 0.0, 0.0, 10.0, 30.0, 20.0, 
    //     20.0, 20.0, -5.0, -5.0, -5.0, -5.0, 20.0, 20.0, 
    //     -10.0, -20.0, -20.0, -20.0, -20.0, -20.0, -20.0, -10.0, 
    //     -20.0, -30.0, -30.0, -40.0, -40.0, -30.0, -30.0, -20.0, 
    //     -30.0, -40.0, -40.0, -50.0, -50.0, -40.0, -40.0, -30.0, 
    //     -40.0, -50.0, -50.0, -60.0, -60.0, -50.0, -50.0, -40.0, 
    //     -60.0, -60.0, -60.0, -60.0, -60.0, -60.0, -60.0, -60.0, 
    //     -80.0, -70.0, -70.0, -70.0, -70.0, -70.0, -70.0, -80.0, 
    // };

    // std::vector<float> kings_end_util = {
    //     -50.0, -30.0, -30.0, -30.0, -30.0, -30.0, -30.0, -50.0, 
    //     -30.0, -25.0, 0.0, 0.0, 0.0, 0.0, -25.0, -30.0, 
    //     -25.0, -20.0, 20.0, 25.0, 25.0, 20.0, -20.0, -25.0, 
    //     -20.0, -15.0, 30.0, 40.0, 40.0, 30.0, -15.0, -20.0, 
    //     -15.0, -10.0, 35.0, 45.0, 45.0, 35.0, -10.0, -15.0, 
    //     -10.0, -5.0, 20.0, 30.0, 30.0, 20.0, -5.0, -10.0, 
    //     -5.0, 0.0, 5.0, 5.0, 5.0, 5.0, 0.0, -5.0, 
    //     -20.0, -10.0, -10.0, -10.0, -10.0, -10.0, -10.0, -20.0, 
    // };

    // std::pair<float, std::string> iterativeDeepening(int maxDepth) {
    //     std::string bestMove = "";
    //     auto start = std::chrono::high_resolution_clock::now();
    //     if (board.sideToMove() == Color("w")) {
    //         float bestScore = -100000000;
    //         for (int depth = 1; depth <= maxDepth; depth++) {
    //             std::pair<float, std::string> result = alphaBetaPruning(depth, -100000000, 100000000);
    //             auto end = std::chrono::high_resolution_clock::now();
    //             auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //             if (duration <= 2000) {
    //                 maxDepth++;
    //             }
    //             if (result.first < bestScore) {
    //                 bestScore = result.first;
    //                 bestMove = result.second;
    //             }
    //         }
    //         // std::cout << "Best Score: " << bestScore << std::endl;
    //         return {bestScore, bestMove};
    //     }
    //     if (board.sideToMove() == Color("b")) {
    //         float bestScore = 100000000;
    //         for (int depth = 1; depth <= maxDepth; depth++) {
    //             std::pair<float, std::string> result = alphaBetaPruning(depth, -100000000, 100000000);
    //             // std::cout << "Depth: " << depth << std::endl;
    //             // std::cout << "Best Score: " << result.first << (bestScore <= 10000000000) << std::endl;
    //             auto end = std::chrono::high_resolution_clock::now();
    //             auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //             if (duration <= 2000) {
    //                 maxDepth++;
    //             }
    //             if (result.first < bestScore) {
    //                 bestScore = result.first;
    //                 bestMove = result.second;
    //             }
    //             if (bestScore <= -100000000 + 130000) {
    //                 // std::cout << "Depth: " << depth << std::endl;
    //                 break;
    //             }
    //         }
    //         // std::cout << "Depth: " << depth << std::endl;
    //         // std::cout << "Best Score: " << bestScore << std::endl;
    //         return {bestScore, bestMove};
    //     }
    //     float bestScore = -100000000;
    //     for (int depth = 1; depth <= maxDepth; depth++) {
    //         std::pair<float, std::string> result = alphaBetaPruning(depth, -100000000, 100000000);
    //         // std::cout << "Depth: " << depth << std::endl;
    //         // std::cout << "Best Score: " << result.first << std::endl;
    //         if (result.first > bestScore) {
    //             bestScore = result.first;
    //             bestMove = result.second;
    //         }
    //         if (bestScore >= 100000000 - 130000) {
    //             std::cout << "Depth: " << depth << std::endl;
    //             break;
    //         }
    //     }
    //     // std::cout << "Depth: " << depth << std::endl;
    //     // std::cout << "Best Score: " << bestScore << std::endl;
    //     return {bestScore, bestMove};
    // }

    std::pair<float, std::string> alphaBetaPruning_flag(int depth, float alpha, float beta, std::chrono::time_point<std::chrono::high_resolution_clock> start, bool& flag, int& nodes, bool root = false) {
        if (!root) {
            if (transpositionTable.find(board.hash()) != transpositionTable.end()) {
                auto find = transpositionTable[board.hash()];
                if (find.second >= depth) {
                    return {find.first, ""};
                }
            }
        }
        if (flag) {
            if (board.sideToMove() == Color("w")) {
                return {100000001, ""};
            } else {
                return {100000001, ""};
            }
        }
        double duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        nodes++;
        if (duration >= 10000) {
            // std::cout << "Hey" << std::endl;
            flag = true;
            if (board.sideToMove() == Color("w")) {
                return {100000001, ""};
            } else {
                return {100000001, ""};
            }
        }
        if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
            if(board.sideToMove() == Color("w")) {
                transpositionTable[board.hash()] = std::make_pair(-100000000 + (100-depth)*100, depth);
                return {-100000000 + (100-depth)*100, ""};
                //change to 1000000-depth should fix it
            } else {
                transpositionTable[board.hash()] = std::make_pair(100000000 - (100-depth)*100, depth);
                return {100000000 - (100-depth)*100, ""};
            }
        }
        if (board.isGameOver().second == GameResult::DRAW) {
            // transpositionTable[board.hash()] = std::make_pair(0, depth);
            if (board.sideToMove() == Color("w")) {
                transpositionTable[board.hash()] = std::make_pair(-1e8 + (100)*100, depth);
                return {-1e8 + (100)*100, ""};
            } else {
                transpositionTable[board.hash()] = std::make_pair(1e8 - (100)*100, depth);
                return {1e8 - (100)*100, ""};
            }
        }
        if (depth == 0) {
            return {evaluate(), ""};
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
                    score = alphaBetaPruning_flag(depth - 1, alpha, beta, start, flag, nodes).first;
                    if (abs(score) >= 100000000) {
                        board.unmakeMove(move);
                        break;
                    }
                }
            } else {
                score = alphaBetaPruning_flag(depth - 1, alpha, beta, start, flag, nodes).first;
                if (abs(score) >= 100000000) {
                    board.unmakeMove(move);
                    break;
                }
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
            // if (depth == 5) {
            //     std::cout << uci::moveToUci(move) << " " << score << std::endl;
            // }
            if (alpha >= beta) {
                break;
            }
            // std::cout << depth << " " << uci::moveToUci(move) << " " << score << std::endl;
            
        }
        if (flag) {
            return {board.sideToMove() == Color("w") ? alpha : beta, bestMove};
        }
        transpositionTable[board.hash()] = std::make_pair(board.sideToMove() == Color("w") ? alpha : beta, depth);
        return {board.sideToMove() == Color("w") ? alpha : beta, bestMove};
    }

    std::pair<float, std::string> alphaBetaPruning(int depth, float alpha, float beta) {
        if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
            if(board.sideToMove() == Color("w")) {
                transpositionTable[board.hash()] = std::make_pair(-100000000 + (100-depth)*100, depth);
                return {-100000000 + (100-depth)*100, ""};
            } else {
                transpositionTable[board.hash()] = std::make_pair(100000000 - (100-depth)*100, depth);
                return {100000000 - (100-depth)*100, ""};
            }
        }
        if (depth == 0) {
            return {evaluate(), ""};
        }
        if (board.isGameOver().second == GameResult::DRAW) {
            // transpositionTable[board.hash()] = std::make_pair(0, depth);
            if (board.sideToMove() == Color("w")) {
                transpositionTable[board.hash()] = std::make_pair(-1e8 + (100)*100, depth);
                return {-1e8 + (100)*100, ""};
            } else {
                transpositionTable[board.hash()] = std::make_pair(1e8 - (100)*100, depth);
                return {1e8 - (100)*100, ""};
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
                    if (abs(score) >= 100000000) {
                        board.unmakeMove(move);
                        break;
                    }
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
            // if (depth == 5) {
            //     std::cout << uci::moveToUci(move) << " " << score << std::endl;
            // }
            if (alpha >= beta) {
                break;
            }
            // std::cout << depth << " " << uci::moveToUci(move) << " " << score << std::endl;
            
        }
        transpositionTable[board.hash()] = std::make_pair(board.sideToMove() == Color("w") ? alpha : beta, depth);
        return {board.sideToMove() == Color("w") ? alpha : beta, bestMove};
    }

    
};


void sendResponse(const std::string& response) {
    std::cout << response << std::endl;
}

void handleUci() {
    sendResponse("id name My_Cute_Engine<3");
    sendResponse("id author Balaji");
    sendResponse("uciok");
}

void handleIsReady() {
    sendResponse("readyok");
}


void handleQuit() {
    // Clean up and exit
    exit(0);
}



int main() {
    std::string command;
    Engine engine = Engine();
    while (std::getline(std::cin, command)) {
        if (command == "uci") {
            handleUci();
        } else if (command == "isready") {
            handleIsReady();
        } else if (command.substr(0, 8) == "position") {
            std::istringstream iss(command);
            std::string token;
            iss >> token; // "position"

            std::string positionType;
            iss >> positionType;

            if (positionType == "startpos") {
                // Set up initial position
                // engine = Engine();
                engine.board = Board();
                iss >> token; // Check if there are more tokens, should be "moves" or end of string
            } else if (positionType == "fen") {
                // Read FEN string
                std::string fen;
                while (iss >> token && token != "moves") {
                    fen += token + " ";
                }
                engine = Engine(fen);
            }
            // Handle moves if present
            if (token == "moves") {
                std::string move;
                while (iss >> move) {
                    engine.board.makeMove(uci::uciToMove(engine.board, move));
                }
            }
        } else if(command.substr(0, 2) == "go") {
            auto move = engine.getBetaMove();
            float eval = engine.getEval();
            // std::cout << "info depth " << move.second << " score cp " << static_cast<int>(eval * 100) << std::endl;
            sendResponse("bestmove " + move.first);
        } else if (command == "quit") {
            handleQuit();
        }
    }
    return 0;
}


// int main() {
//     std::string command;
//     Engine engine;
//     while (std::getline(std::cin, command)) {
//         if (command == "uci") {
//             handleUci();
//         } else if (command == "isready") {
//             handleIsReady();
//         } else if (command.substr(0, 8) == "position") {
//             std::istringstream iss(command);
//             std::string token;
//             iss >> token; // "position"

//             std::string positionType;
//             iss >> positionType;

//             if (positionType == "startpos") {
//                 // Set up initial position
//                 engine = Engine();
//                 iss >> token; // Check if there are more tokens, should be "moves" or end of string
//             } else if (positionType == "fen") {
//                 // Read FEN string
//                 std::string fen;
//                 while (iss >> token && token != "moves") {
//                     fen += token + " ";
//                 }
//                 engine = Engine(fen);
//             }
//             // Handle moves if present
//             if (token == "moves") {
//                 std::string move;
//                 while (iss >> move) {
//                     engine.board.makeMove(uci::uciToMove(engine.board, move));
//                 }
//             }
//         } else if(command.substr(0, 2) == "go") {
//             sendResponse("bestmove " + engine.getBestMove());
//         } else if (command == "quit") {
//             handleQuit();
//         }
//     }
//     return 0;
// }
