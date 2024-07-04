import json
import numpy as np

Pawns = [
    [0, 0, 0, 0, 0, 0, 0, 0],
    [50, 50, 50, 50, 50, 50, 50, 50],
    [10, 10, 20, 30, 30, 20, 10, 10],
    [5, 5, 10, 25, 25, 10, 5, 5],
    [0, 0, 0, 20, 20, 0, 0, 0],
    [5, -5, -10, 0, 0, -10, -5, 5],
    [5, 10, 10, -20, -20, 10, 10, 5],
    [0, 0, 0, 0, 0, 0, 0, 0]
]

pawns_util = np.zeros(64)

for i in range(7, -1, -1):
	for j in range(8):
		pawns_util[i*8+j] = Pawns[7-i][j]

PawnsEnd = [
    [0, 0, 0, 0, 0, 0, 0, 0],
    [80, 80, 80, 80, 80, 80, 80, 80],
    [50, 50, 50, 50, 50, 50, 50, 50],
    [30, 30, 30, 30, 30, 30, 30, 30],
    [20, 20, 20, 20, 20, 20, 20, 20],
    [10, 10, 10, 10, 10, 10, 10, 10],
    [10, 10, 10, 10, 10, 10, 10, 10],
    [0, 0, 0, 0, 0, 0, 0, 0]
]

pawns_end_util = np.zeros(64)

for i in range(7, -1, -1):
	for j in range(8):
		pawns_end_util[i*8+j] = PawnsEnd[7-i][j]

Rooks = [
    [0, 0, 0, 0, 0, 0, 0, 0],
    [5, 10, 10, 10, 10, 10, 10, 5],
    [-5, 0, 0, 0, 0, 0, 0, -5],
    [-5, 0, 0, 0, 0, 0, 0, -5],
    [-5, 0, 0, 0, 0, 0, 0, -5],
    [-5, 0, 0, 0, 0, 0, 0, -5],
    [-5, 0, 0, 0, 0, 0, 0, -5],
    [0, 0, 0, 5, 5, 0, 0, 0]
]

rooks_util = np.zeros(64)

for i in range(7, -1, -1):
	for j in range(8):
		rooks_util[i*8+j] = Rooks[7-i][j]

Knights = [
    		[-50,-40,-30,-30,-30,-30,-40,-50],
			[-40,-20,  0,  0,  0,  0,-20,-40],
			[-30,  0, 10, 15, 15, 10,  0,-30],
			[-30,  5, 15, 20, 20, 15,  5,-30],
			[-30,  0, 15, 20, 20, 15,  0,-30],
			[-30,  5, 10, 15, 15, 10,  5,-30],
			[-40,-20,  0,  5,  5,  0,-20,-40],
			[-50,-40,-30,-30,-30,-30,-40,-50]
]

knights_util = np.zeros(64)

for i in range(7, -1, -1):
	for j in range(8):
		knights_util[i*8+j] = Knights[7-i][j]


Bishops = [
            [-20,-10,-10,-10,-10,-10,-10,-20],
			[-10,  0,  0,  0,  0,  0,  0,-10],
			[-10,  0,  5, 10, 10,  5,  0,-10],
			[-10,  5,  5, 10, 10,  5,  5,-10],
			[-10,  0, 10, 10, 10, 10,  0,-10],
			[-10, 10, 10, 10, 10, 10, 10,-10],
			[-10,  5,  0,  0,  0,  0,  5,-10],
			[-20,-10,-10,-10,-10,-10,-10,-20],
]

bishops_util = np.zeros(64)

for i in range(7, -1, -1):
	for j in range(8):
		bishops_util[i*8+j] = Bishops[7-i][j]


Queens = [
            [-20,-10,-10, -5, -5,-10,-10,-20],
			[-10,  0,  0,  0,  0,  0,  0,-10],
			[-10,  0,  5,  5,  5,  5,  0,-10],
			[-5,   0,  5,  5,  5,  5,  0, -5],
			[0,    0,  5,  5,  5,  5,  0, -5],
			[-10,  5,  5,  5,  5,  5,  0,-10],
			[-10,  0,  5,  0,  0,  0,  0,-10],
			[-20,-10,-10, -5, -5,-10,-10,-20]
]

queens_util = np.zeros(64)

for i in range(7, -1, -1):
	for j in range(8):
		queens_util[i*8+j] = Queens[7-i][j]

KingStart = [
			[-80, -70, -70, -70, -70, -70, -70, -80], 
			[-60, -60, -60, -60, -60, -60, -60, -60], 
			[-40, -50, -50, -60, -60, -50, -50, -40], 
			[-30, -40, -40, -50, -50, -40, -40, -30], 
			[-20, -30, -30, -40, -40, -30, -30, -20], 
			[-10, -20, -20, -20, -20, -20, -20, -10], 
			[20,  20,  -5,  -5,  -5,  -5,  20,  20], 
			[20,  30,  10,   0,   0,  10,  30,  20]
]

king_start_util = np.zeros(64)

for i in range(7, -1, -1):
	for j in range(8):
		king_start_util[i*8+j] = KingStart[7-i][j]


KingEnd = [
    		[-20, -10, -10, -10, -10, -10, -10, -20],
			[-5,   0,   5,   5,   5,   5,   0,  -5],
			[-10, -5,   20,  30,  30,  20,  -5, -10],
			[-15, -10,  35,  45,  45,  35, -10, -15],
			[-20, -15,  30,  40,  40,  30, -15, -20],
			[-25, -20,  20,  25,  25,  20, -20, -25],
			[-30, -25,   0,   0,   0,   0, -25, -30],
			[-50, -30, -30, -30, -30, -30, -30, -50]
]

king_end_util = np.zeros(64)

for i in range(7, -1, -1):
	for j in range(8):
		king_end_util[i*8+j] = KingEnd[7-i][j]


data = {
    "Pawns": Pawns,
    "PawnsEnd": PawnsEnd,
    "Rooks": Rooks,
    "Queens": Queens,
    "KingStart": KingStart,
    "KingEnd": KingEnd,
    "Knights": Knights,
    "Bishops": Bishops
}


# with open('SquareUtility.json', 'w') as file:
#     json.dump(data, file)

np.save('pawns_util.npy', pawns_util)
np.save('pawns_end_util.npy', pawns_end_util)
np.save('rooks_util.npy', rooks_util)
np.save('queens_util.npy', queens_util)
np.save('king_start_util.npy', king_start_util)
np.save('king_end_util.npy', king_end_util)
np.save('knights_util.npy', knights_util)
np.save('bishops_util.npy', bishops_util)
