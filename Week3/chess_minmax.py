import chess
import math

def alphabet(fen, depth):
    board = chess.Board(fen)
    return alpha_beta_pruning(board, -math.inf, math.inf, depth)

def alpha_beta_pruning(board, alpha, beta, depth, max_player=True):
    if board.is_checkmate():
        if board.turn == max_player:
            return (1000000, list())
        else:
            return (-1000000, list())
    if board.is_stalemate():
        return 0, []
    if depth == 0:
        map = board.piece_map()
        ref = {'p': 1, 'n': 3, 'b': 3, 'r': 5, 'q': 9, 'k': 100}
        count = dict()
        pieces = ['k', 'q', 'r', 'b', 'n', 'p']
        for piece in pieces:
            count[piece] = 0
            count[piece.upper()] = 0
        score = 0
        for key in map:
            count[str(map[key])] += 1
        for piece in pieces:
            score += (count[piece.upper()] - count[piece]) * ref[piece]
        return (score, list())
    # if board.fen() == '4rr1k/7b/5K1R/R7/8/8/8/8 w - - 2 2':
    #     print('here')
    if max_player:
        max_eval = -1000000
        best_move = []
        for move in ordered_moves(board.fen()):
            new = board.copy()
            new.push(move)
            eval, cmove = alpha_beta_pruning(new, alpha, beta, depth - 1, False)
            # print(new.fen(), eval, cmove, depth, alpha, beta)
            max_eval = max(max_eval, eval)
            if max_eval == eval:
                best_move = [move, ] + cmove 
            alpha = max(alpha, eval)
            if beta <= alpha:
                break
        return max_eval, best_move
    else:
        min_eval = 1000000
        best_move = []
        for move in ordered_moves(board.fen()):
            new = board.copy()
            new.push(move)
            eval, cmove = alpha_beta_pruning(new, alpha, beta, depth - 1, True)
            # print(new.fen(), eval, cmove, depth, alpha, beta)
            min_eval = min(min_eval, eval)
            if min_eval == eval:
                # print(cmove)
                best_move = [move, ] + cmove
            beta = min(beta, eval)
            if beta <= alpha:
                break
        return min_eval, best_move
    
def ordered_moves(fen):
    head_board = chess.Board(fen)
    legal_moves = head_board.legal_moves
    moves_dict = dict()
    for move in legal_moves:
        head_board.push(move)
        moves_dict[move] = len(list(head_board.legal_moves))
        head_board.pop()
    sorted_moves = sorted(moves_dict.items(), key=lambda x: x[1])
    return [x[0] for x in sorted_moves]