import json
mate_in_2 = json.load(open('mate_in_2.json'))

import matplotlib.pyplot as plt
import numpy as np
from tqdm import tqdm
import time
import chess
import chess.svg
from my_chess_engine import Engine

data = np.zeros(351)
start = time.time()
correct = 0
for i, puzzle in enumerate(tqdm(mate_in_2)):
    start_ = time.time()
    engine = Engine(puzzle)
    result = engine.alphabet(3)
    if engine.board.is_checkmate():
        correct += 1
    else:
        print(puzzle)
    data[i] = time.time() - start_
        # print(chess.svg.board(engine.board, lastmove=engine.board.peek()))
print(f'Time: {time.time() - start} s')
print(f'Correct: {correct}/{len(mate_in_2)}')
plt.plot(np.arange(351), data)
plt.show()