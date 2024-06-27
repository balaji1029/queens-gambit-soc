import sys
import chess
import chess.engine
from my_chess_engine import Engine

# Initialize the engine
engine = chess.engine.SimpleEngine.popen_uci("/path/to/your/engine_binary")  # Replace with your engine binary path

def uci_loop():
    board = chess.Board()

    while True:
        line = input().strip()

        if line == "quit":
            break
        elif line == "uci":
            print("id name Engine1")
            print("id author Balaji")
            print("uciok")
        elif line == "isready":
            print("readyok")
        elif line.startswith("position"):
            # Parse the position command
            parts = line.split()
            if len(parts) > 1 and parts[1] == "startpos":
                board = chess.Board()
                if len(parts) > 2 and parts[2] == "moves":
                    for move in parts[3:]:
                        board.push(chess.Move.from_uci(move))
            elif len(parts) > 1 and parts[1] == "fen":
                # Parse FEN string and set board position
                fen = " ".join(parts[2:8])
                board.set_fen(fen)
                if len(parts) > 8 and parts[8] == "moves":
                    for move in parts[9:]:
                        board.push(chess.Move.from_uci(move))
        elif line.startswith("go"):
            # Parse the go command and start searching
            # For simplicity, a random move is generated here
            engine = Engine(board.fen())
            move = engine.get_move(3)[1]  # Replace with actual move generation logic
            print(f"bestmove {move.uci()}")
        elif line.startswith("position"):
            # Handle setting up the board position
            pass

    engine.quit()

if __name__ == "__main__":
    uci_loop()
