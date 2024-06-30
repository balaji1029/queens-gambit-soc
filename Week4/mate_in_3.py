import json
mate_in_3 = json.load(open('mate_in_3.json'))

from tqdm import tqdm
import matplotlib.pyplot as plt
import numpy as np
import time
import chess.svg
from my_chess_engine import Engine

start = time.time()
correct = 0
data = np.zeros(489)
for i, puzzle in enumerate(tqdm(mate_in_3)):
    start_ = time.time()
    engine = Engine(puzzle)
    result = engine.alphabet(5)
    if engine.board.is_checkmate():
        correct += 1
    else:
        print(puzzle)
    data[i] = time.time() - start_
        # print(chess.svg.board(engine.board, lastmove=engine.board.peek()))
print(f'Time: {time.time() - start} s')
print(f'Correct: {correct}/{len(mate_in_3)}')
plt.plot(np.arange(489), data)
plt.show()