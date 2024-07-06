#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include "surge-master/src/position.h"
#include "surge-master/src/tables.h"
#include "surge-master/src/types.h"

// using namespace chess;

int main() {
    initialise_all_databases();
    zobrist::initialise_zobrist_keys();
    // Board board;
    // // Get the piece map
    // board.makeMove(uci::uciToMove(board, "e2e4"));
    // for (int i=0; i<8; i++) {
    //     for (int j=0; j<8; j++) {
    //         std::cout << board.at(Square(i*8+j)) << " ";
    //     }
    //     std::cout << std::endl;
    // }

    // surge::Game game;
    // game.reset();  // Start a new game
    // Position::positi

    // // Display initial board state
    // std::cout << game.toFEN() << std::endl;

    // // Make some moves leading up to castling
    // game.move("e2", "e4");
    // game.move("e7", "e5");
    // game.move("g1", "f3");
    // game.move("b8", "c6");
    // game.move("f1", "c4");
    // game.move("g8", "f6");

    // // Castling move: Kingside for white
    // if (game.isLegalMove("e1", "g1")) {
    //     game.move("e1", "g1");
    //     std::cout << "White castles kingside!" << std::endl;
    // }

    // // Display the board state after castling
    // std::cout << game.toFEN() << std::endl;

    // return 0;

    Position p;
    Position::set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", p);
    std::cout << p; 
  
    MoveList<WHITE> list(p);
  
    for(Move m : list) {
        std::cout << m << "\n";
    }
    
    return 0;
}