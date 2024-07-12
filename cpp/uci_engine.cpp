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
  
template<class T> using ordered_set = tree<T, null_type, custom_compare<T>, rb_tree_tag, tree_order_statistics_node_update>;

using namespace chess;

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

    ordered_set<std::pair<Move, float>> getOrderedMoves() {
        auto moves = getLegalMoves();
        ordered_set<std::pair<Move, float>> orderedMoves;
        for (const auto& move : moves) {
            board.makeMove(move);
            if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
                board.unmakeMove(move);
                ordered_set<std::pair<Move, float>> movie;
                movie.insert({move, MAX_EVAL});
                return movie;
            } else if (board.inCheck()) {
                if (board.sideToMove() == Color("w")) {
                    orderedMoves.insert({move, MAX_EVAL});
                } else {
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
                    if (piece != 5) white_eval += piece_values[piece] + (piece_util[piece][i] / 100.0f);
                    if (piece == 0) white_pawns++;
                    else if (piece == 2) white_bishops++;
                    else if (piece == 5) white_king = i;
                } else { // Black pieces
                    black_pieces++;
                    int black_piece = piece - 6;
                    if (black_piece != 5) black_eval += piece_values[black_piece] + (piece_util[black_piece][mirrored_i] / 100.0f);
                    if (black_piece == 0) black_pawns++;
                    else if (black_piece == 2) black_bishops++;
                    else if (black_piece == 5) black_king = mirrored_i;
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

    // float PieceDifference(const Board& board) {
    //     map<Piece, float> PIECE_VALUES {
    //     {Piece::WHITEPAWN, 1},
    //     {Piece::WHITEKNIGHT, 2.9},
    //     {Piece::WHITEBISHOP, 3.2},
    //     {Piece::WHITEROOK, 5},
    //     {Piece::WHITEQUEEN, 9.75},
    //     {Piece::BLACKPAWN, 1},
    //     {Piece::BLACKKNIGHT, 2.9},
    //     {Piece::BLACKBISHOP, 3.2},
    //     {Piece::BLACKROOK, 5},
    //     {Piece::BLACKQUEEN, 9.75}
    // };

    // map<Piece, array<int, 64>> SQUARE_PIECE_VALUES {
    // {Piece::WHITEPAWN, {
    //     0,   0,   0,   0,   0,   0,   0,   0,
    //     20,  50,  40,  20,  20,  40,  50,  20,
    //     30,  10,  20,  25,  25,  20,  10,  30,
    //     5,   5,   10,  40,  40,  10,  5,   5,
    //     0,   0,   0,   20,  20,  0,   0,   0,
    //     5,   5,   10,  0,   0,   10,  5,   5,
    //     50,  50,  50,  50,  50,  50,  50,  50,
    //     0,   0,   0,   0,   0,   0,   0,   0
    // }},
    // {Piece::WHITEKNIGHT, {
    //     -50, -40, -30, -30, -30, -30, -40, -50,
    //     -40, -20, 0,   0,   0,   0,   -20, -40,
    //     -30, 0,   20,  15,  15,  20,  0,   -30,
    //     -30, 5,   15,  20,  20,  15,  5,   -30,
    //     -30, 5,   15,  20,  20,  15,  5,   -30,
    //     -30, 5,   10,  15,  15,  10,  5,   -30,
    //     -40, -20, 10,  5,   5,   10,  -20, -40,
    //     -50, -40, -30, -30, -30, -30, -40, -50
    // }},
    // {Piece::WHITEBISHOP, {
    //     -20, -10, -10, -10, -10, -10, -10, -20,
    //     -10,  10,  0,   0,   0,   0,   10,  -10,
    //     -10,  10,  10,  10,  10,  10,  10,  -10,
    //     -10,  0,   10,  10,  10,  10,  0,   -10,
    //     -10,  10,  5,   10,  10,  5,   10,  -10,
    //     -10,  0,   5,   10,  10,  5,   0,   -10,
    //     -10,  0,   0,   0,   0,   0,   0,   -10,
    //     -20, -10, -10, -10, -10, -10, -10, -20
    // }},
    // {Piece::WHITEROOK, {
    //     0,   0,   15,  15,  15,  15,  0,   0,
    //     -5,  0,   0,   0,   0,   0,   0,   -5,
    //     -5,  0,   0,   0,   0,   0,   0,   -5,
    //     -5,  0,   0,   0,   0,   0,   0,   -5,
    //     -5,  0,   0,   0,   0,   0,   0,   -5,
    //     -5,  0,   0,   0,   0,   0,   0,   -5,
    //     5,   10,  10,  10,  10,  10,  10,  5,
    //     5,   5,   5,   5,   5,   5,   5,   5
    // }},
    // {Piece::WHITEQUEEN, {
    //     -20, -10, -10, -5,  -5,  -10, -10, -20,
    //     -10,  0,   0,   0,   0,   0,   0,   -10,
    //     -10,  0,   5,   5,   5,   5,   0,   -10,
    //     -5,   0,   5,   5,   5,   5,   0,   -5,
    //     0,    0,   5,   5,   5,   5,   0,   0,
    //     -10,  0,   5,   5,   5,   5,   0,   -10,
    //     -10,  0,   0,   0,   0,   0,   0,   -10,
    //     -20, -10, -10, -5,  -5,  -10, -10, -20
    // }},
    // // Black pieces (mirrored vertically)
    // {Piece::BLACKPAWN, {
    //     0,   0,   0,   0,   0,   0,   0,   0,
    //     50,  50,  50,  50,  50,  50,  50,  50,
    //     5,   5,   10,  0,   0,   10,  5,   5,
    //     0,   0,   0,   20,  20,  0,   0,   0,  
    //     5,   5,   10,  40,  40,  10,  5,   5,
    //     30,  10,  20,  25,  25,  20,  10,  30,
    //     20,  50,  40,  20,  20,  40,  50,  20,
    //     0,   0,   0,   0,   0,   0,   0,   0
    // }},
    // {Piece::BLACKKNIGHT, {
    //     -50, -40, -30, -30, -30, -30, -40, -50,
    //     -40, -20, 10,  5,   5,   10,  -20, -40,
    //     -30, 5,   10,  15,  15,  10,  5,   -30,
    //     -30, 5,   15,  20,  20,  15,  5,   -30,
    //     -30, 5,   15,  20,  20,  15,  5,   -30,
    //     -30, 0,   20,  15,  15,  20,  0,   -30,
    //     -40, -20, 0,   0,   0,   0,   -20, -40,
    //     -50, -40, -30, -30, -30, -30, -40, -50
    // }},
    // {Piece::BLACKBISHOP, {
    //     -20, -10, -10, -10, -10, -10, -10, -20,
    //     -10,  0,   0,   0,   0,   0,   0,   -10,
    //     -10,  0,   5,   5,   5,   5,   0,   -10,
    //     -10,  5,   10,  10,  10,  10,  5,   -10,
    //     -10,  0,   10,  10,  10,  10,  0,   -10,
    //     -10,  10,  10,  10,  10,  10,  10,  -10,
    //     -10,  10,  0,   0,   0,   0,   10,  -10,
    //     -20, -10, -10, -10, -10, -10, -10, -20
    // }},
    // {Piece::BLACKROOK, {
    //     5,   10,  10,  10,  10,  10,  10,  5,
    //     5,   5,   5,   5,   5,   5,   5,   5,
    //     -5,  0,   0,   0,   0,   0,   0,   -5,
    //     -5,  0,   0,   0,   0,   0,   0,   -5,
    //     -5,  0,   0,   0,   0,   0,   0,   -5,
    //     -5,  0,   0,   0,   0,   0,   0,   -5,
    //     0,   0,   15,  15,  15,  15,  0,   0,
    //     0,   0,   0,   0,   0,   0,   0,   0
    // }},
    // {Piece::BLACKQUEEN, {
    //     -20, -10, -10, -5,  -5,  -10, -10, -20,
    //     -10,  0,   5,   5,   5,   5,   0,   -10,
    //     -10,  0,   5,   5,   5,   5,   0,   -10,
    //     -5,   0,   5,   5,   5,   5,   0,   -5,
    //     0,    0,   5,   5,   5,   5,   0,   0,
    //     -10,  0,   0,   0,   0,   0,   0,   -10,
    //     -10,  0,   0,   0,   0,   0,   0,   -10,
    //     -20, -10, -10, -5,  -5,  -10, -10, -20
    // }}
    
    // };

    // float white_points = 0;
    // float black_points = 0;

    // for (int i=0; i<64; i++){

    //     int rownumber = 1 + i%8;
    //     int columnnumber = 1 + i/8;

    //     int squarenumber = (rownumber-1)* 8 + (columnnumber-1);

    //     Square squarecheck = Square(i);
    //     Piece piece = board.at(squarecheck);

    //     if (piece == Piece::NONE || piece == Piece::BLACKKING || piece == Piece::WHITEKING){
    //         continue;
    //     }

    //     float piecevalue = PIECE_VALUES[piece];
        
    //     Color piececolor = piece.color();

    //     float pieceatsquarevalue = SQUARE_PIECE_VALUES[piece][squarenumber];


    //     if (piececolor == Color::WHITE){
    //         float value=piecevalue*(1+pieceatsquarevalue/200);
    //         white_points += value;
    //     }
    //     else{
    //         float value=piecevalue*(1+pieceatsquarevalue/200);
    //         black_points += value;
    //     }
    
    // } 


    // if (abs(white_points - black_points)<=0.001){
    //     return 0;
    // }  
    // else{
    //     return (white_points - black_points) / 5;
    // }
    // }

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
                std::pair<float, std::string> result = alphaBetaPruning_flag(depth, -MAX_EVAL, MAX_EVAL, start, flag, nodes, max_time, true);
                if (flag) {
                    break;
                }
                auto end = std::chrono::high_resolution_clock::now();
                float duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                float duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();

                bestScore = result.first;
                bestMove = result.second;
                if (bestScore >= MAX_EVAL - 10000) {
                    break;
                }
                if (duration > max_time) {
                    break;
                }
                std::cout << "info depth " << depth 
                << " score cp " << static_cast<int>(bestScore * 100) 
                << " time " << static_cast<int>(duration) 
                << " nodes " << nodes 
                << " pv";
                std::vector<Move> moves(depth);
                for (int i=0; i<depth; i++) {
                    // std::cout << "Heyyyyyy " << depth << std::endl;
                    // std::cout << board << std::endl;
                    if (i == 0) {
                        std::cout << " " << bestMove;
                        moves[0] = uci::uciToMove(board, bestMove);
                        board.makeMove(uci::uciToMove(board, bestMove));
                    } else {
                        std::pair<float, std::string> result = alphaBetaPruning_flag(1, -MAX_EVAL, MAX_EVAL, start, flag, nodes, max_time, true);
                        std::cout << " " << result.second;
                        moves[i] = uci::uciToMove(board, result.second);
                        board.makeMove(uci::uciToMove(board, result.second));
                    }
                }
                std::cout << std::endl;
                for (int i=depth-1; i>=0; i--) {
                    board.unmakeMove(moves[i]);
                }
                depth++;
            }
            return {bestMove, depth};
        }
        float bestScore = MAX_EVAL;
        std::string bestMove = "";
        float duration = 0;
        while (duration <= max_time) {
            auto start_ = std::chrono::high_resolution_clock::now();
            bool flag = false;
            int nodes = 0;
            std::pair<float, std::string> result = alphaBetaPruning_flag(depth, -MAX_EVAL, MAX_EVAL, start, flag, nodes, true);
            if (flag) {
                break;
            }
            auto end = std::chrono::high_resolution_clock::now();
            float duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            float duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
            bestScore = result.first;
            bestMove = result.second;
            if (bestScore <= -MAX_EVAL + 10000) {
                break;
            }
            if (duration > max_time) {
                break;
            }
            std::cout << "info depth " << depth 
            << " score cp " << static_cast<int>(bestScore * 100) 
            << " time " << static_cast<int>(duration) 
            << " nodes " << nodes
            << " pv";
            std::vector<Move> moves(depth);
            for (int i=0; i<depth; i++) {
                // std::cout << "Heyyyyyy" << std::endl;
                if (i == 0) {
                    std::cout << " " << bestMove;
                    moves[0] = uci::uciToMove(board, bestMove);
                    board.makeMove(uci::uciToMove(board, bestMove));
                } else {
                    std::pair<float, std::string> result = alphaBetaPruning_flag(1, -MAX_EVAL, MAX_EVAL, start, flag, nodes, true);
                    std::cout << " " << result.second;
                    moves[i] = uci::uciToMove(board, result.second);
                    board.makeMove(uci::uciToMove(board, result.second));
                }
            }
            std::cout << std::endl;
            for (int i=depth-1; i>=0; i--) {
                board.unmakeMove(moves[i]);
            }
            // std::cout << board << std::endl;
            depth++;
        }
        return {bestMove, depth};
    }

    float getEval() {
        return transpositionTable[board.hash()].first;
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
                int value2;
                if (iss >> key >> value1 >> value2) {
                    uint64_t hash = std::stoull(key);
                    transpositionTable[hash] = {value1, value2};
                }
            }
            file.close();
        }
    }
    
    std::pair<float, std::string>alphaBetaPruning_flag(int depth, float alpha, float beta, std::chrono::time_point<std::chrono::high_resolution_clock> start, bool& flag, int& nodes, int max_time = 10000, bool root = false) {
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
        float duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        nodes++;
        if (duration >= max_time) {
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
                transpositionTable[board.hash()] = std::make_pair(-MAX_EVAL + (100-depth)*100, depth);
                return {-MAX_EVAL + (100-depth)*100, ""};
            } else {
                transpositionTable[board.hash()] = std::make_pair(MAX_EVAL - (100-depth)*100, depth);
                return {MAX_EVAL - (100-depth)*100, ""};
            }
        }
        if (board.isGameOver().second == GameResult::DRAW) {
            // transpositionTable[board.hash()] = std::make_pair(0, depth);
            if (board.sideToMove() == Color("w")) {
                transpositionTable[board.hash()] = std::make_pair(-1e8 + (100)*100, depth);
                return {0, ""};
            } else {
                transpositionTable[board.hash()] = std::make_pair(1e8 - (100)*100, depth);
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
                std::pair<float, int> transposition = transpositionTable[board.hash()];
                if (transposition.second >= depth) {
                    score = transposition.first;
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
            return {board.sideToMove() == Color("w") ? alpha : beta, bestMove};
        }
        transpositionTable[board.hash()] = std::make_pair(bestScore, depth);
        return {bestScore, bestMove};
    }

    std::pair<float, std::string> alphaBetaPruning(int depth, float alpha, float beta) {
        if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
            if(board.sideToMove() == Color("w")) {
                transpositionTable[board.hash()] = std::make_pair(-MAX_EVAL + (100-depth)*100, depth);
                return {-MAX_EVAL + (100-depth)*100, ""};
            } else {
                transpositionTable[board.hash()] = std::make_pair(MAX_EVAL - (100-depth)*100, depth);
                return {MAX_EVAL - (100-depth)*100, ""};
            }
        }
        if (depth == 0) {
            return {evaluate(), ""};
        }
        if (board.isGameOver().second == GameResult::DRAW) {
            if (board.sideToMove() == Color("w")) {
                transpositionTable[board.hash()] = std::make_pair(0, depth);
                return {0, ""};
            } else {
                transpositionTable[board.hash()] = std::make_pair(0, depth);
                return {0, ""};
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
        transpositionTable[board.hash()] = std::make_pair(board.sideToMove() == Color("w") ? alpha : beta, depth);
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
        } else if(command.substr(0, 11) == "go movetime"){
            std::istringstream iss(command);
            std::string goCommand, movetimeKeyword;
            int maxTimeInMillis = 10000;
            iss >> goCommand >> movetimeKeyword >> maxTimeInMillis;
            std::cout << "Max time per move: " << maxTimeInMillis << std::endl;
            auto move = engine.getBetaMove(maxTimeInMillis);
            float eval = engine.getEval();
            // std::cout << "info depth " << move.second << " score cp " << static_cast<int>(eval * 100) << std::endl;
            sendResponse("bestmove " + move.first);
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
