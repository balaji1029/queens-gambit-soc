#include "engine.h"


#define MAX_TIME 10000
#define MAX_EVAL 100000000

using namespace chess;

void Engine::setFen(const std::string& fen) {
    board = Board(fen);
}

std::vector<Move> Engine::getLegalMoves() {
    std::vector<Move> legalMoves;
    Movelist moves;
    movegen::legalmoves(moves, board);
    for (const auto& move : moves) {
        legalMoves.push_back(move);
    }
    return legalMoves;
}

ordered_set<std::pair<Move, float>> Engine::getOrderedMoves() {

    if (orderedMoveTable.find(board.hash()) != orderedMoveTable.end()) {
        return orderedMoveTable[board.hash()];
    }

    Movelist moves;
    movegen::legalmoves(moves, board);
    ordered_set<std::pair<Move, float>> orderedMoves;
    for (auto& move : moves) {
        board.makeMove(move);
        if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
            board.unmakeMove(move);
            ordered_set<std::pair<Move, float>> movie;
            // move.setScore(MAX_EVAL);
            if (board.sideToMove() == Color("w")) {
                movie.insert({move, MAX_EVAL});
            } else {
                movie.insert({move, -MAX_EVAL});
            }
            orderedMoveTable[board.hash()] = movie;
            return movie;
        } else if (board.inCheck()) {
            if (board.sideToMove() == Color("w")) {
                orderedMoves.insert({move, -MAX_EVAL});
            } else {
                orderedMoves.insert({move, MAX_EVAL});
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
    orderedMoveTable[board.hash()] = orderedMoves;
    // std::cout << "Ordering: " << moves.size() << " " << orderedMoves.size() << std::endl;
    return orderedMoves;
}

void Engine::makeMove(const Move& move) {
    board.makeMove(move);
}

void Engine::undoMove(const Move& move) {
    board.unmakeMove(move);
}

float Engine::evaluate() {

    // if (evalTable.find(board.hash()) != evalTable.end()) {
    //     return evalTable[board.hash()];
    // }

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

    for (int i = 0; i < 64; i+=32) {
        eval_square(&board, i, &white_pieces, &black_pieces, &white_eval, &black_eval, &white_pawns, &black_pawns, &white_bishops, &black_bishops, &white_king, &black_king);
    }

    const float* king_util = (white_pieces + black_pieces > 8) ? kings_start_util.data() : kings_end_util.data();

    white_eval += king_util[white_king];
    black_eval += king_util[black_king];

    if (white_bishops >= 2) white_eval += 50;
    if (black_bishops >= 2) black_eval += 50;

    score = white_eval - black_eval;
    // evalTable[board.hash()] = score;
    return score;
}

std::pair<std::string, int> Engine::getBetaMove(int max_time = 10000) {
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
            std::pair<float, std::string> result = alphaBetaPruning_flag(depth, -MAX_EVAL, MAX_EVAL, start, flag, nodes, true, max_time);
            if (flag) {
                break;
            }
            auto end = std::chrono::high_resolution_clock::now();
            float duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            float duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();

            bestScore = result.first;
            bestMove = result.second;
            if (abs(bestScore) >= MAX_EVAL - 10000000) {
                std::cout << "Idk" << std::endl;
                std::cout << bestScore << " " << MAX_EVAL - 10000 << std::endl;
                break;
            }
            if (duration > max_time) {
                std::cout << "Duration" << std::endl;
                break;
            }
            std::cout << "info depth " << depth 
            << " score cp " << static_cast<int>(bestScore) 
            << " time " << static_cast<int>(duration) 
            << " nodes " << nodes << std::endl; 
            // << " pv";
            // std::cout << "---" << bestMove << "---" << std::endl;
            // std::vector<Move> moves;
            // for (int i=0; i<depth; i++) {
            //     // std::cout << "Heyyyyyy " << depth << std::endl;
            //     // std::cout << board << std::endl;
            //     if (i == 0) {
            //         std::cout << " " << bestMove;
            //         moves.push_back(uci::uciToMove(board, bestMove));
            //         board.makeMove(uci::uciToMove(board, bestMove));
            //     } else {
            //         std::pair<float, std::string> tempresult = alphaBetaPruning_flag(1, -MAX_EVAL, MAX_EVAL, start, flag, nodes, true, max_time);
            //         if (tempresult.second == "") {
            //             break;
            //         }
            //         std::cout << " " << tempresult.second;
            //         moves.push_back(uci::uciToMove(board, tempresult.second));
            //         board.makeMove(uci::uciToMove(board, tempresult.second));
            //     }
            // }
            // std::cout << std::endl;
            // for (int i=moves.size()-1; i>=0; i--) {
            //     board.unmakeMove(moves[i]);
            // }
            if (depth > 50) {
                break;
            }
            depth++;
        }
        // std::cout << "Returning" << std::endl;
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
        std::pair<float, std::string> result = alphaBetaPruning_flag(depth, -MAX_EVAL, MAX_EVAL, start, flag, nodes, true, max_time);
        // std::cout << "Reached here" << std::endl;
        if (flag) {
            // std::cout << "Flag" << std::endl;
            break;
        }
        auto end = std::chrono::high_resolution_clock::now();
        float duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        float duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
        bestScore = result.first;
        bestMove = result.second;
        // std::cout << "---" << bestMove << "---" << std::endl;
        if (abs(bestScore) >= MAX_EVAL - 10000000) {
            // std::cout << bestScore << " " << -MAX_EVAL + 10000 << std::endl;
            break;
        }
        if (duration > max_time) {
            // std::cout << "Duration" << std::endl;
            break;
        }
        std::cout << "info depth " << depth 
        << " score cp " << static_cast<int>(bestScore) 
        << " time " << static_cast<int>(duration) 
        << " nodes " << nodes << std::endl; 
        // << " pv";
        // std::vector<Move> moves;
        // for (int i=0; i<depth; i++) {
        //     // std::cout << "Heyyyyyy" << std::endl;
        //     if (i == 0) {
        //         std::cout << " " << bestMove;
        //         moves.push_back(uci::uciToMove(board, bestMove));
        //         // std::cout << board << " " << bestMove << std::endl;
        //         board.makeMove(uci::uciToMove(board, bestMove));
        //         // std::cout << "Reached here" << std::endl;
        //     } else {
        //         std::pair<float, std::string> result = alphaBetaPruning_flag(1, -MAX_EVAL, MAX_EVAL, start, flag, nodes, true, max_time);
        //         std::cout << " " << result.second;
        //         if (result.second == "") {
        //             break;
        //         }
        //         moves[i] = uci::uciToMove(board, result.second);
        //         board.makeMove(uci::uciToMove(board, result.second));
        //     }
        // }
        // std::cout << std::endl;
        // for (int i=moves.size()-1; i>=0; i--) {
        //     board.unmakeMove(moves[i]);
        // }
        // std::cout << board << std::endl;
        if (depth > 50) {
            break;
        }
        depth++;
    }
    // std::cout << "Returning" << std::endl;
    // save_transposition_table();
    return {bestMove, depth};
}

float Engine::getEval() {
    return transpositionTable[board.hash()].first;
}

std::pair<float, std::string> Engine::alphaBetaPruning_flag(int depth, float alpha, float beta, std::chrono::time_point<std::chrono::high_resolution_clock> start, bool& flag, int& nodes, bool root, int max_time = 10000) {
    // std::cout << "root" << " " << root << std::endl;
    // if (!root) {
    //     if (transpositionTable.find(board.hash()) != transpositionTable.end()) {
    //         auto find = transpositionTable[board.hash()];
    //         if (find.second >= depth) {
    //             // std::cout << "Hereeeeee" << std::endl;
    //             return {find.first, ""};
    //         }
    //     }
    // }
    // if (flag) {
    //     if (board.sideToMove() == Color("w")) {
    //         return {110000000, ""};
    //     } else {
    //         return {110000000, ""};
    //     }
    // }
    // float duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    // nodes++;
    // // std::cout << duration << " " << max_time << std::endl;
    // if (duration >= max_time) {
    //     // std::cout << "Hey" << std::endl;
    //     flag = true;
    //     if (board.sideToMove() == Color("w")) {
    //         return {110000000, ""};
    //     } else {
    //         return {110000000, ""};
    //     }
    // }
    // if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
    //     if(board.sideToMove() == Color("w")) {
    //         transpositionTable[board.hash()] = std::make_pair(-MAX_EVAL + (100-depth)*100, depth);
    //         return {-MAX_EVAL + (100-depth)*100, ""};
    //     } else {
    //         transpositionTable[board.hash()] = std::make_pair(MAX_EVAL - (100-depth)*100, depth);
    //         return {MAX_EVAL - (100-depth)*100, ""};
    //     }
    // }
    // if (board.isGameOver().second == GameResult::DRAW) {
    //     // transpositionTable[board.hash()] = std::make_pair(0, depth);
    //     if (board.sideToMove() == Color("w")) {
    //         transpositionTable[board.hash()] = std::make_pair(-1e8 + (100)*100, depth);
    //         return {0, ""};
    //     } else {
    //         transpositionTable[board.hash()] = std::make_pair(1e8 - (100)*100, depth);
    //         return {0, ""};
    //     }
    // }
    // if (depth == 0) {
    //     return {evaluate(), ""};
    // }
    // auto moves = getOrderedMoves();
    // int i = 0;
    // std::string bestMove;
    // float bestScore = board.sideToMove() == Color("w") ? -MAX_EVAL : MAX_EVAL;
    // // std::cout << board << std::endl;
    // // std::cout << "Size: " << moves.size() << std::endl;
    // // std::cout << "Size comp: " << movies.size() << std::endl;
    // for (const auto& movie : moves) {
    //     // std::cout << "---" << move << "---" << std::endl;
    //     Move move = movie.first;
    //     if (i == 0) {
    //         bestMove = uci::moveToUci(move);
    //     }
    //     // std::cout << "Reached" << std::endl;
    //     board.makeMove(move);
    //     // std::cout << "Reached" << std::endl;
    //     float score;
    //     if (transpositionTable.find(board.hash()) != transpositionTable.end()) {
    //         std::pair<float, int> transposition = transpositionTable[board.hash()];
    //         if (transposition.second >= depth) {
    //             score = transposition.first;
    //         } else {
    //             score = alphaBetaPruning_flag(depth - 1, alpha, beta, start, flag, nodes, false, max_time).first;
    //             if (abs(score) >= MAX_EVAL) {
    //                 board.unmakeMove(move);
    //                 break;
    //             }
    //         }
    //     } else {
    //         score = alphaBetaPruning_flag(depth - 1, alpha, beta, start, flag, nodes, false, max_time).first;
    //         if (abs(score) >= MAX_EVAL) {
    //             board.unmakeMove(move);
    //             break;
    //         }
    //     }
    //     // if (depth == 2) {
    //     //     std::cout << score << std::endl;
    //     // }
    //     board.unmakeMove(move);
    //     if (board.sideToMove() == Color("w")) {
    //         if (score > bestScore) {
    //             bestScore = score;
    //             bestMove = uci::moveToUci(move);
    //             alpha = std::max(alpha, score);
    //         }
    //     } else {
    //         // std::cout << score << " " << bestScore << std::endl;
    //         if (score < bestScore) {
    //             // std::cout << "Reached here" << std::endl;
    //             bestScore = score;
    //             bestMove = uci::moveToUci(move);
    //             beta = std::min(beta, score);
    //         }
    //     }
    //     // if (depth == 5) {
    //     //     std::cout << move << " " << score << " " << bestMove << " " << bestScore << std::endl;
    //     // }
    //     if (alpha >= beta) {
    //         break;
    //     }
    //     i++;
    // }
    // if (flag) {
    //     return {board.sideToMove() == Color("w") ? alpha : beta, bestMove};
    // }
    // transpositionTable[board.hash()] = std::make_pair(bestScore, depth);
    // // std::cout << "---" << bestMove << "---" << std::endl;
    // return {bestScore, bestMove};

    if (!root) {
        if (transpositionTable.find(board.hash()) != transpositionTable.end()) {
            auto find = transpositionTable[board.hash()];
            if (find.second >= depth) {
                return {find.first, ""};
            }
        }
    }

    if (flag) {
        return {110000000, ""};
    }

    float duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    nodes++;
    if (duration >= max_time) {
        flag = true;
        return {110000000, ""};
    }

    if (board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE) {
        if(board.sideToMove() == Color("w")) {
            transpositionTable[board.hash()] = std::make_pair(-MAX_EVAL + (100-depth)*100, depth);
            return {-MAX_EVAL + (100-depth)*100000, ""};
        } else {
            transpositionTable[board.hash()] = std::make_pair(MAX_EVAL - (100-depth)*100, depth);
            return {MAX_EVAL - (100-depth)*100000, ""};
        }
    }

    if (board.isGameOver().second == GameResult::DRAW) {
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
            
            auto transposition = transpositionTable[board.hash()];
            if (transposition.second >= depth) {
                score = transposition.first;
            } else {
                score = alphaBetaPruning_flag(depth - 1, alpha, beta, start, flag, nodes, false, max_time).first;
                if (abs(score) >= MAX_EVAL) {
                    board.unmakeMove(move);
                    break;
                }
            }
        } else {
            score = alphaBetaPruning_flag(depth - 1, alpha, beta, start, flag, nodes, false, max_time).first;
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

        if (alpha >= beta) {
            break;
        }

        i++;
    }

    if (flag) {
        return {11000000, bestMove};
    }

    transpositionTable[board.hash()] = std::make_pair(bestScore, depth);
    return {bestScore, bestMove};
}

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
            iss >> token;

            std::string positionType;
            iss >> positionType;

            if (positionType == "startpos") {
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
            sendResponse("bestmove " + move.first);
        } else if(command.substr(0, 2) == "go") {
            auto move = engine.getBetaMove();
            sendResponse("bestmove " + move.first);
        } else if (command == "quit") {
            handleQuit();
        } else if (command == "len") {
            std::cout << engine.transpositionTable.size() << std::endl;
        }
    }
    return 0;
}

// int main() {
//     Engine engine = Engine();
//     engine.getBetaMove(10000);
// }

// int main() {
//     std::ifstream file("m8n4.txt");
//         if (file.is_open()) {
//             std::string line;
//             int correct = 0; 
//             while (std::getline(file, line)) {
//                 std::istringstream iss(line);
//                 std::string fen = line;
//                 Engine engine = Engine(fen);
//                 int i = 0;
//                 while (engine.board.isGameOver().second == GameResult::NONE) {
//                     auto move = engine.getBetaMove(100000);
//                     engine.board.makeMove(uci::uciToMove(engine.board, move.first));
//                     i++;
//                     if (i == 9) {
//                         break;
//                     }
//                 }
//                 if (engine.board.isGameOver().second == GameResult::NONE) {
//                     std::cout << "Failed" << std::endl;
//                     std::cout << fen << std::endl;
//                     std::cout << engine.board << std::endl;
//                     break;
//                 }
//                 correct++;
//                 std::cout << correct << "/462" << std::endl;
//             }
//             file.close();
//         }
// }