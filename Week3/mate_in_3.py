import json
mate_in_3 = json.load(open('mate_in_3.json'))

from tqdm import tqdm
import time
import chess
import chess.svg
from my_chess_engine import Engine

start = time.time()
correct = 0
for puzzle in tqdm(mate_in_3):
    engine = Engine(puzzle)
    result = engine.alphabet(5)
    if engine.board.is_checkmate():
        correct += 1
    else:
        print(puzzle)
        # print(chess.svg.board(engine.board, lastmove=engine.board.peek()))
print(f'Time: {time.time() - start} s')
print(f'Correct: {correct}/{len(mate_in_3)}')
