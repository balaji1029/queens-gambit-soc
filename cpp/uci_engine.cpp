#include "engine.cpp"
#include <iostream>
#include <string>

void send_uci() {
    std::cout << "id name MyCPPEngine" << std::endl;
    std::cout << "id author Balaji" << std::endl;
    std::cout << "uciok" << std::endl;
}

void send_readyok() {
    std::cout << "readyok" << std::endl;
}

void send_bestmove(const std::string& move) {
    std::cout << "bestmove " << move << std::endl;
}

int main() {
    std::string command;
    Engine engine = Engine();

    while (true) {
        std::getline(std::cin, command);

        if (command == "uci") {
            send_uci();
        } else if (command == "isready") {
            send_readyok();
        } else if (command.rfind("position", 0) == 0) {
            // Handle setting up the position
            // Example: "position startpos" or "position fen <fen-string>"
            engine.setFen(command.substr(9));
        } else if (command.rfind("go", 0) == 0) {
            // Start searching for the best move
            // For now, just a dummy move
            std::cout << "info depth 5" << std::endl;
            auto bestMove = engine.getAlphaMove(5);
            std::cout << "bestmove " << bestMove << std::endl;
            send_bestmove(bestMove);
        } else if (command == "stop") {
            // Stop the search

        } else if (command == "quit") {
            break;
        }
    }

    return 0;
}