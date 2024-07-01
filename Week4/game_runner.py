import matplotlib.pyplot as plt
import cv2 as cv
import chess.svg
from my_chess_engine import Engine

engine = Engine()
board = open("board.svg", "w")
board.write(chess.svg.board(engine.board))
board.close()
img = cv.imread("board.svg")
plt.imshow(img)
while not engine.board.is_game_over():
    engine.make_move(engine.get_move(5)[1])
    board = open("board.svg", "w")
    board.write(chess.svg.board(engine.board))
    board.close()
    try:
        plt.close()
    except:
        pass
    img = cv.imread("board.svg")
    plt.imshow(img)