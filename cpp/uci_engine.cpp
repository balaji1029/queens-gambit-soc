#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include "chess-library-master/include/chess.hpp"
#include <cmath>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <fstream>
#include <sstream>
#include <thread>
#include <ext/pb_ds/assoc_container.hpp> 
#include <ext/pb_ds/tree_policy.hpp> 

#define MAX_TIME 10000
#define MAX_EVAL 100000000

using namespace __gnu_pbds;

template<class T>
struct custom_compare {
    bool operator()(const T& a, const T& b) const {
        return a.second <= b.second;
    }
};    
const float piece_values[6] = {100, 300, 325, 500, 900, 0}; // Values for pawn, knight, bishop, rook, queen, king

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

template<class T> using ordered_set = tree<T, null_type, custom_compare<T>, rb_tree_tag, tree_order_statistics_node_update>;

using namespace chess;

void eval_square(Board* board, int j, int* white_pieces, int* black_pieces, float* white_eval, float* black_eval, int* white_pawns, int* black_pawns, int* white_bishops, int* black_bishops, int* white_king, int* black_king) {
    for (int i = 0 + j; i-j < 32; i++) {
        int piece = board->at(i);
        if (piece != 12) {
            int mirrored_i = 63 - i;
            if (piece < 6) { // White pieces
                (*white_pieces)++;
                if (piece != 5) (*white_eval) += piece_values[piece] + (piece_util[piece][i]);
                if (piece == 0) (*white_pawns)++;
                else if (piece == 2) (*white_bishops)++;
                else if (piece == 5) (*white_king) = i;
            } else { // Black pieces
                (*black_pieces)++;
                int black_piece = piece - 6;
                if (black_piece != 5) (*black_eval) += piece_values[black_piece] + (piece_util[black_piece][mirrored_i]);
                if (black_piece == 0) (*black_pawns)++;
                else if (black_piece == 2) (*black_bishops)++;
                else if (black_piece == 5) (*black_king) = mirrored_i;
            }
        }
    }
}

class Engine {
public:
    Board board;
    std::unordered_map<uint64_t, std::tuple<float, std::string, int>> transpositionTable;
    Engine(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
        : board(fen) {
            // load_transposition_table();
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

    ordered_set<std::pair<Move, float>> getOrderedMoves() {
        auto moves = getLegalMoves();
        ordered_set<std::pair<Move, float>> orderedMoves;
        for (const auto& move : moves) {
            board.makeMove(move);
            if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
                board.unmakeMove(move);
                ordered_set<std::pair<Move, float>> movie;
                // move.setScore(MAX_EVAL);
                if (board.sideToMove() == Color("w")) {
                    orderedMoves.insert({move, -MAX_EVAL});
                } else {
                    orderedMoves.insert({move, MAX_EVAL});
                }
                return movie;
            } else if (board.inCheck()) {
                if (board.sideToMove() == Color("w")) {
                    // move.setScore(-MAX_EVAL);
                    orderedMoves.insert({move, MAX_EVAL});
                } else {
                    // move.setScore(MAX_EVAL);
                    orderedMoves.insert({move, -MAX_EVAL});
                }
            } else {
                if (board.sideToMove() == Color("w")) {
                    orderedMoves.insert({move, evaluate()});
                } else {
                    orderedMoves.insert({move, -evaluate()});
                }
            }
            board.unmakeMove(move);
        }
        return orderedMoves;
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
        float score = 0;
        int white_pawns = 0, black_pawns = 0;
        int white_pieces = 0, black_pieces = 0;
        int white_bishops = 0, black_bishops = 0;
        float white_eval = 0, black_eval = 0;
        int white_king = 0, black_king = 0;

        // for (int i = 0; i < 64; ++i) {
        //     int piece = board.at(i);
        //     if (piece != 12) {
        //         int mirrored_i = 63 - i;
        //         if (piece < 6) { // White pieces
        //             white_pieces++;
        //             if (piece != 5) white_eval += piece_values[piece] + (piece_util[piece][i]);
        //             if (piece == 0) white_pawns++;
        //             else if (piece == 2) white_bishops++;
        //             else if (piece == 5) white_king = i;
        //         } else { // Black pieces
        //             black_pieces++;
        //             int black_piece = piece - 6;
        //             if (black_piece != 5) black_eval += piece_values[black_piece] + (piece_util[black_piece][mirrored_i]);
        //             if (black_piece == 0) black_pawns++;
        //             else if (black_piece == 2) black_bishops++;
        //             else if (black_piece == 5) black_king = mirrored_i;
        //         }
        //     }
        // }

        // std::vector<std::thread> threads;
        // std::cout << "White pieces: " << white_pieces << std::endl;
        for (int i = 0; i < 64; i+=32) {
            eval_square(&board, i, &white_pieces, &black_pieces, &white_eval, &black_eval, &white_pawns, &black_pawns, &white_bishops, &black_bishops, &white_king, &black_king);
            // threads.emplace_back(eval_square, &board, i, &white_pieces, &black_pieces, &white_eval, &black_eval, &white_pawns, &black_pawns, &white_bishops, &black_bishops, &white_king, &black_king);
            // t.join();
        }
        // for (auto& t : threads) {
        //     t.join();
        // }
        // std::thread t(eval_square, &board, 0, &white_pieces, &black_pieces, &white_eval, &black_eval, &white_pawns, &black_pawns, &white_bishops, &black_bishops, &white_king, &black_king);
        // }
        // std::cout << "White pieces: " << white_pieces << std::endl;

        const float* king_util = (white_pieces + black_pieces > 8) ? kings_start_util.data() : kings_end_util.data();

        white_eval += king_util[white_king];
        black_eval += king_util[black_king];

        if (white_bishops >= 2) white_eval += 50;
        if (black_bishops >= 2) black_eval += 50;

        score = white_eval - black_eval;
        return score;
    }

    std::pair<std::string, int> getBetaMove(int max_time = 10000) {
        auto start = std::chrono::high_resolution_clock::now();
        int depth = 1;
        if (board.sideToMove() == Color("w")) {
            float bestScore = -MAX_EVAL;
            std::string bestMove = "";
            float duration = 0;
            while (duration <= max_time) {
                auto start_ = std::chrono::high_resolution_clock::now();
                bool flag = false;
                int nodes = 0;
                std::pair<float, std::string> result = alphaBetaPruning_flag(depth, -MAX_EVAL, MAX_EVAL, start, flag, nodes, max_time);
                if (flag) {
                    break;
                }
                auto end = std::chrono::high_resolution_clock::now();
                float duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                float duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();

                bestScore = result.first;
                bestMove = result.second;
                if (abs(bestScore) >= MAX_EVAL - 10000000) {
                    break;
                }
                if (duration > max_time) {
                    break;
                }
                std::cout << "info depth " << depth 
                << " score cp " << static_cast<int>(bestScore * 100) 
                << " time " << static_cast<int>(duration) 
                << " nodes " << nodes << std::endl;
                // << " pv";
                // // std::cout << "---" << bestMove << "---" << std::endl;
                // std::vector<Move> moves(depth);
                // for (int i=0; i<depth; i++) {
                //     // std::cout << "Heyyyyyy " << depth << std::endl;
                //     // std::cout << board << std::endl;
                //     if (i == 0) {
                //         std::cout << " " << bestMove;
                //         moves[0] = uci::uciToMove(board, bestMove);
                //         board.makeMove(uci::uciToMove(board, bestMove));
                //     } else {
                //         std::pair<float, std::string> result = alphaBetaPruning_flag(1, -MAX_EVAL, MAX_EVAL, start, flag, nodes, max_time);
                //         std::cout << " " << result.second;
                //         moves[i] = uci::uciToMove(board, result.second);
                //         board.makeMove(uci::uciToMove(board, result.second));
                //     }
                // }
                // std::cout << std::endl;
                // for (int i=depth-1; i>=0; i--) {
                //     board.unmakeMove(moves[i]);
                // }
                depth++;
            }
            // save_transposition_table();
            return {bestMove, depth};
        }
        float bestScore = MAX_EVAL;
        std::string bestMove = "";
        float duration = 0;
        while (duration <= max_time) {
            auto start_ = std::chrono::high_resolution_clock::now();
            bool flag = false;
            int nodes = 0;
            // std::cout << "Reached here" << std::endl;
            std::pair<float, std::string> result = alphaBetaPruning_flag(depth, -MAX_EVAL, MAX_EVAL, start, flag, nodes, true);
            // std::cout << "Reached here" << std::endl;
            if (flag) {
                break;
            }
            auto end = std::chrono::high_resolution_clock::now();
            float duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            float duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
            bestScore = result.first;
            bestMove = result.second;
            // std::cout << "---" << bestMove << "---" << std::endl;
            if (abs(bestScore) >= MAX_EVAL - 10000000) {
                break;
            }
            if (duration > max_time) {
                break;
            }
            std::cout << "info depth " << depth 
            << " score cp " << static_cast<int>(bestScore * 100) 
            << " time " << static_cast<int>(duration) 
            << " nodes " << nodes << std::endl;
            // << " pv";
            // std::vector<Move> moves(depth);
            // for (int i=0; i<depth; i++) {
            //     // std::cout << "Heyyyyyy" << std::endl;
            //     if (i == 0) {
            //         std::cout << " " << bestMove;
            //         moves[0] = uci::uciToMove(board, bestMove);
            //         // std::cout << board << " " << bestMove << std::endl;
            //         board.makeMove(uci::uciToMove(board, bestMove));
            //         // std::cout << "Reached here" << std::endl;
            //     } else {
            //         std::pair<float, std::string> result = alphaBetaPruning_flag(1, -MAX_EVAL, MAX_EVAL, start, flag, nodes, true);
            //         std::cout << " " << result.second;
            //         moves[i] = uci::uciToMove(board, result.second);
            //         board.makeMove(uci::uciToMove(board, result.second));
            //     }
            // }
            // std::cout << std::endl;
            // for (int i=depth-1; i>=0; i--) {
            //     board.unmakeMove(moves[i]);
            // }
            // std::cout << board << std::endl;
            depth++;
        }
        // save_transposition_table();
        return {bestMove, depth};
    }

    float getEval() {
        return std::get<0>(transpositionTable[board.hash()]);
    }

    std::string getBestMove() {
        return alphaBetaPruning(5, -MAX_EVAL, MAX_EVAL).second;
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
                std::string value2;
                int value3;
                if (iss >> key >> value1 >> value2 >> value3) {
                    uint64_t hash = std::stoull(key);
                    transpositionTable[hash] = {value1, value2, value3};
                }
            }
            file.close();
        }
    }
    
    void save_transposition_table() {
        std::ofstream file("transposition_table.txt");
        for (const auto& [key, value] : transpositionTable) {
            file << key << " " << std::get<0>(value) << " " << std::get<1>(value) << " " << std::get<2>(value) << std::endl;
        }
        file.close();
        // std::cout << "Transposition table saved" << std::endl;
    }

    std::pair<float, std::string>alphaBetaPruning_flag(int depth, float alpha, float beta, std::chrono::time_point<std::chrono::high_resolution_clock> start, bool& flag, int& nodes, int max_time = 10000) {
        // std::cout << "root" << " " << root << std::endl;
        if (transpositionTable.find(board.hash()) != transpositionTable.end()) {
            auto find = transpositionTable[board.hash()];
            if (std::get<2>(find) >= depth) {
                // std::cout << "Hereeeeee" << std::endl;
                return {std::get<0>(find), std::get<1>(find)};
            }
        }
        if (flag) {
            return {110000000, ""};
        }
        float duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        nodes++;
        if (duration >= max_time) {
            // std::cout << "Hey" << std::endl;
            flag = true;
            return {110000000, ""};
        }
        if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
            if(board.sideToMove() == Color("w")) {
                transpositionTable[board.hash()] = {-MAX_EVAL + (100-depth)*100, "", depth};
                return {-MAX_EVAL + (100-depth)*100000, ""};
            } else {
                transpositionTable[board.hash()] = {MAX_EVAL - (100-depth)*100, "", depth};
                return {MAX_EVAL - (100-depth)*100000, ""};
            }
        }
        if (board.isGameOver().second == GameResult::DRAW) {
            // transpositionTable[board.hash()] = std::make_pair(0, depth);
            if (board.sideToMove() == Color("w")) {
                transpositionTable[board.hash()] = {0, "", depth};
                return {0, ""};
            } else {
                transpositionTable[board.hash()] = {0, "", depth};
                return {0, ""};
            }
        }
        if (depth == 0) {
            return {evaluate(), ""};
        }
        auto moves = getOrderedMoves();
        int i = 0;
        std::string bestMove;
        float bestScore = board.sideToMove() == Color("w") ? -MAX_EVAL : MAX_EVAL;
        for (const auto& movie : moves) {
            Move move = movie.first;
            if (i == 0) {
                bestMove = uci::moveToUci(move);
            }
            board.makeMove(move);
            float score;
            if (transpositionTable.find(board.hash()) != transpositionTable.end()) {
                auto transposition = transpositionTable[board.hash()];
                if (std::get<2>(transposition) >= depth) {
                    score = std::get<0>(transposition);
                } else {
                    score = alphaBetaPruning_flag(depth - 1, alpha, beta, start, flag, nodes).first;
                    if (abs(score) >= MAX_EVAL) {
                        board.unmakeMove(move);
                        break;
                    }
                }
            } else {
                score = alphaBetaPruning_flag(depth - 1, alpha, beta, start, flag, nodes).first;
                if (abs(score) >= MAX_EVAL) {
                    board.unmakeMove(move);
                    break;
                }
            }
            board.unmakeMove(move);
            if (board.sideToMove() == Color("w")) {
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = uci::moveToUci(move);
                    alpha = std::max(alpha, score);
                }
            } else {
                if (score < bestScore) {
                    bestScore = score;
                    bestMove = uci::moveToUci(move);
                    beta = std::min(beta, score);
                }
            }
            // if (depth == 5) {
            //     std::cout << move << " " << score << " " << bestMove << " " << bestScore << std::endl;
            // }
            if (alpha >= beta) {
                break;
            }
            i++;
        }
        if (flag) {
            return {110000000, ""};
        }
        transpositionTable[board.hash()] = {bestScore, bestMove, depth};
        // std::cout << "---" << bestMove << "---" << std::endl;
        return {bestScore, bestMove};
    }

    std::pair<float, std::string> alphaBetaPruning(int depth, float alpha, float beta) {
        if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
            if(board.sideToMove() == Color("w")) {
                transpositionTable[board.hash()] = {-MAX_EVAL + (100-depth)*100, "", depth};
                return {-MAX_EVAL + (100-depth)*100, ""};
            } else {
                transpositionTable[board.hash()] = {MAX_EVAL - (100-depth)*100, "", depth};
                return {MAX_EVAL - (100-depth)*100, ""};
            }
        }
        if (depth == 0) {
            return {evaluate(), ""};
        }
        if (board.isGameOver().second == GameResult::DRAW) {
            if (board.sideToMove() == Color("w")) {
                transpositionTable[board.hash()] = {0, "", depth};
                return {0, ""};
            } else {
                transpositionTable[board.hash()] = {0, "", depth};
                return {0, ""};
            }
        }
        auto moves = getLegalMoves();
        std::string bestMove = uci::moveToUci(moves[0]);
        for (const auto& move : moves) {
            board.makeMove(move);
            float score;
            if (transpositionTable.find(board.hash()) != transpositionTable.end()) {
                auto transposition = transpositionTable[board.hash()];
                if (std::get<2>(transposition) >= depth) {
                    score = std::get<0>(transposition);
                } else {
                    score = alphaBetaPruning(depth - 1, alpha, beta).first;
                    if (abs(score) >= MAX_EVAL) {
                        board.unmakeMove(move);
                        break;
                    }
                }
            } else {
                score = alphaBetaPruning(depth - 1, alpha, beta).first;
            }
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
        }
        transpositionTable[board.hash()] = {board.sideToMove() == Color("w") ? alpha : beta, bestMove, depth};
        return {(board.sideToMove() == Color("w")) ? alpha : beta, bestMove};
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
        // std::cout << command.substr(0, 11) << " " << (command.substr(0, 11) == "go movetime") << std::endl;
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
                engine.setFen(fen);
            }
            // Handle moves if present
            if (token == "moves") {
                std::string move;
                // std::cout << "Token found" << std::endl;
                while (iss >> move) {
                    // std::cout << engine.board << move << std::endl;
                    engine.board.makeMove(uci::uciToMove(engine.board, move));
                    // std::cout << engine.board << std::endl;
                }
            }
        } else if(command.substr(0, 11) == "go movetime"){
            // std::cout << engine.board << std::endl;
            std::istringstream iss(command);
            std::string goCommand, movetimeKeyword;
            int maxTimeInMillis = 10000;
            iss >> goCommand >> movetimeKeyword >> maxTimeInMillis;
            std::cout << "Max time per move: " << maxTimeInMillis << std::endl;
            auto move = engine.getBetaMove(maxTimeInMillis);
            // float eval = engine.getEval();
            // std::cout << "info depth " << move.second << " score cp " << static_cast<int>(eval * 100) << std::endl;
            sendResponse("bestmove " + move.first);
        } else if(command.substr(0, 2) == "go") {
            // std::cout << engine.board << std::endl;
            auto move = engine.getBetaMove();
            // float eval = engine.getEval();
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
