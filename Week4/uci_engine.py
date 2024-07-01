#!/usr/bin/env python3

import sys
import time
from my_chess_engine import Engine
import chess

class UCIEngine:
    def __init__(self):
        self.engine = Engine()
        self.log_file = open("uci_engine.log", "a")

    def log(self, message):
        print(message, file=self.log_file)
        self.log_file.flush()

    def handle_command(self, command):
        self.log(f"Received command: {command}")
        parts = command.strip().split()
        if not parts:
            return

        if parts[0] == "uci":
            self.uci()
        elif parts[0] == "isready":
            self.isready()
        elif parts[0] == "ucinewgame":
            self.ucinewgame()
        elif parts[0] == "position":
            self.position(parts[1:])
        elif parts[0] == "go":
            self.go(parts[1:])
        elif parts[0] == "stop":
            self.stop()
        elif parts[0] == "quit":
            self.quit()

    def uci(self):
        print("id name MyChessEngine")
        print("id author Balaji")
        print("uciok")
        self.log("Sent UCI response")

    def isready(self):
        time.sleep(0.1)  # Small delay to avoid timing issues
        print("readyok")
        self.log("Sent readyok response")

    def ucinewgame(self):
        self.engine.reset()
        self.log("Game reset")

    def position(self, parts):
        self.log(f"Setting position with parts: {parts}")
        if parts[0] == "startpos":
            self.engine.set_fen(chess.STARTING_FEN)
            if len(parts) > 1 and parts[1] == "moves":
                moves = parts[2:]
                for move in moves:
                    self.engine.make_move(move)
        elif parts[0] == "fen":
            fen = " ".join(parts[1:])
            self.engine.set_fen(fen)

    def go(self, parts):
        self.log("Starting move calculation...")
        best_move = self.engine.get_move(5)[1]
        if best_move:
            print(f"bestmove {best_move.uci()}")
            self.log(f"Sent bestmove {best_move.uci()}")
        else:
            self.log("No valid moves found.")

    def stop(self):
        self.log("Received stop command")

    def quit(self):
        self.log("Quitting engine")
        self.log_file.close()
        sys.exit(0)

def main():
    engine = UCIEngine()
    while True:
        try:
            command = input()
            engine.handle_command(command)
        except EOFError:
            break

if __name__ == "__main__":
    main()
