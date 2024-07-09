file = open('some.txt', 'r')
lines = file.readlines()
file.close()

# count = 0
# text = ''
# for line in lines:
#     if 'White' in line or 'Black' in line:
#         text += line.strip().split(' ')[1] + '\n'

# file = open('some.txt', 'w')
# file.write(text)
# file.close()

import chess
import chess.pgn

board = chess.Board()

pgn = ''

count = 0
for move in lines:
    if count % 2 == 0:
        pgn += str(count//2 + 1) + '. ' + board.san(chess.Move.from_uci(move.strip())) + ' '
    else:
        pgn += board.san(chess.Move.from_uci(move.strip())) + ' '
    count += 1
    board.push_uci(move.strip())

print(pgn)