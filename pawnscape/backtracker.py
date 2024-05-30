# Depth First Search

from engine import Board, Engine

def backtracker(board, depth):
    if depth == 0:
        return None
    for move in board.possible_moves():
        board.make_move(move)
        if board.check_win() is not None:
            board.undo_move(move)
            return move
        move = backtracker(board, depth-1)
        board.undo_move(move)
        if move is not None:
            return move
    return None

if __name__=='__main__':
    board = Board()
    backtracker(board, 16)