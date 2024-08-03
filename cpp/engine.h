#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
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
#include "chess-library-master/include/chess.hpp"

using namespace chess;
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
    class chess::Board board;
    class std::unordered_map<unsigned long, std::pair<float, int>, std::hash<unsigned long>, std::equal_to<unsigned long>, std::allocator<std::pair<unsigned long const, std::pair<float, int> > > > transpositionTable;
    class std::unordered_map<unsigned long, ordered_set<std::pair<Move, float>>, std::hash<unsigned long>, std::equal_to<unsigned long>, std::allocator<std::pair<unsigned long const, ordered_set<std::pair<Move, float> > > > > orderedMoveTable;
    // class std::unordered_map<unsigned long, float, std::hash<unsigned long>, std::equal_to<unsigned long>, std::allocator<std::pair<unsigned long const, float> > > evalTable;
    Engine(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
    : board(fen) {
        // load_transposition_table();
    };
    void setFen(const std::string &);
    class std::vector<chess::Move, std::allocator<chess::Move> > getLegalMoves();
    ordered_set<std::pair<Move, float>> getOrderedMoves();
    class std::shared_ptr<Engine> getChild(const class chess::Move &);
    void makeMove(const class chess::Move &);
    void undoMove(const class chess::Move &);
    float evaluate();
    struct std::pair<std::string, int> getBetaMove(int);
    float getEval();
    std::string getBestMove();
    
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
    
    void save_transposition_table() {
        std::ofstream file("transposition_table.txt");
        for (const auto& [key, value] : transpositionTable) {
            file << key << " " << value.first << " " << value.second << std::endl;
        }
        file.close();
    }

    struct std::pair<float, std::string> alphaBetaPruning_flag(int, float, float, struct std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long, std::ratio<1, 1000000000> > >, bool &, int &, bool, int);
    struct std::pair<float, std::string> alphaBetaPruning(int, float, float);
};

#endif