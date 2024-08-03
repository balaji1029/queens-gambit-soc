import chess
board = chess.Board()

with open('pgn.txt') as file:
    for line in file.readlines():
        print(board.parse_san(line.strip()), end=' ')
        board.push_san(line.strip())