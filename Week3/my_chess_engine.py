from turtle import end_fill
import chess
import chess.svg
import math
import random
import numpy as np

from numpy import mat

storage = dict()

class Engine:

    pieces = np.load('pieces.npy')
    en_passant = np.load('en_passant.npy')

    def __init__(self, fen='rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq'):
        self.board = chess.Board(fen)
        self.hash = self.get_hash()

    def get_child(self, move):
        new = Engine(self.board.fen())
        new.make_move(move)
        return new

    def get_legal_moves(self):
        return list(self.board.legal_moves)
    
    @staticmethod
    def get_index(piece):
        piece_map = {'p': 0, 'n': 1, 'b': 2, 'r': 3, 'q': 4, 'k': 5, 'P': 6, 'N': 7, 'B': 8, 'R': 9, 'Q': 10, 'K': 11}
        return piece_map[str(piece)]
    
    def update_hash(self, move):
        hash = self.hash >> 5
        piece = self.board.piece_at(move.from_square)
        # Update hash for moving piece
        hash ^= self.pieces[move.from_square][self.get_index(piece)]
        # Update hash for capturing piece
        if self.board.is_capture(move):
            hash ^= self.pieces[move.to_square][self.get_index(self.board.piece_at(move.to_square))]
        self.board.push(move)
        # Update hash for en passant
        if self.board.is_en_passant(move):
            hash ^= self.en_passant[move.to_square % 8]
        # Update hash for castling rights
        if self.board.has_kingside_castling_rights(chess.WHITE):
            hash <<= 1
            hash ^= 1
        else:
            hash <<= 1
        if self.board.has_queenside_castling_rights(chess.WHITE):
            hash <<= 1
            hash ^= 1
        else:
            hash <<= 1
        if self.board.has_kingside_castling_rights(chess.BLACK):
            hash <<= 1
            hash ^= 1
        else:
            hash <<= 1
        if self.board.has_queenside_castling_rights(chess.BLACK):
            hash <<= 1
            hash ^= 1
        else:
            hash <<= 1
        # Update hash for turn
        hash <<= 1
        if self.board.turn == chess.WHITE:
            hash ^= 1
        return hash
    
    # Zobrist Hashing
    def get_hash(self):
        hash = 0
        map = self.board.piece_map()
        for key in map:
            piece = map[key]
            if piece is not None:
                hash ^= self.pieces[key][self.get_index(piece)]
        if self.board.ep_square is not None:
            hash ^= self.en_passant[self.board.ep_square % 8]
        # Castling rights
        if self.board.has_kingside_castling_rights(chess.WHITE):
            hash <<= 1
            hash ^= 1
        else:
            hash <<= 1
        if self.board.has_queenside_castling_rights(chess.WHITE):
            hash <<= 1
            hash ^= 1
        else:
            hash <<= 1
        if self.board.has_kingside_castling_rights(chess.BLACK):
            hash <<= 1
            hash ^= 1
        else:
            hash <<= 1
        if self.board.has_queenside_castling_rights(chess.BLACK):
            hash <<= 1
            hash ^= 1
        else:
            hash <<= 1
        # Turn
        if self.board.turn == chess.WHITE:
            hash <<= 1
            hash ^= 1
        else:
            hash <<= 1
        return hash

    def make_move(self, move):
        self.hash = self.update_hash(move)

    def undo_move(self):
        move = self.board.pop()
        self.hash = self.update_hash(move)
    

    def get_ordered_moves(self):
        legal_moves = self.get_legal_moves()
        moves_dict = dict()
        for move in legal_moves:
            self.make_move(move)
            moves_dict[move] = math.log(len(self.get_legal_moves()))
            self.undo_move()
        for move in legal_moves:
            self.make_move(move)
            if self.board.is_checkmate():
                moves_dict[move] += -math.inf
            self.undo_move()
        for move in legal_moves:
            self.make_move(move)
            if self.board.is_check():
                moves_dict[move] += -math.inf
            self.undo_move()
        return sorted(moves_dict, key=lambda x: moves_dict[x])

    def eval(self, max_player):
        if self.board.is_checkmate():
            if max_player:
                return -math.inf
            else:
                return math.inf
        if self.board.is_stalemate():
            return 0
        map = self.board.piece_map()
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
        return score

    def alpha_beta_pruning(self, alpha, beta, depth, max_player):
        global storage
        if depth == 0 or self.board.is_game_over():
            return self.eval(max_player), []
        if max_player:
            max_eval = -math.inf
            for move in self.get_ordered_moves():
                # print(move, self.get_ordered_moves())
                new = self.get_child(move)
                if new.hash in storage:
                    print('here')
                    eval, moves = storage[new.hash]
                else:
                    eval, moves = new.alpha_beta_pruning(alpha, beta, depth - 1, False)
                if eval >= max_eval:
                    moves = [move,] + moves
                max_eval = max(max_eval, eval)
                alpha = max(alpha, eval)
                if beta <= alpha:
                    break
            storage[self.hash] = (max_eval, moves)
            return max_eval, moves
        else:
            min_eval = math.inf
            for move in self.get_ordered_moves():
                new = self.get_child(move)
                if new.hash in storage:
                    print('here')
                    eval, moves = storage[new.hash]
                else:
                    eval, moves = new.alpha_beta_pruning(alpha, beta, depth - 1, True)
                if eval <= min_eval:
                    moves = [move,] + moves
                min_eval = min(min_eval, eval)
                beta = min(beta, eval)
                if beta <= alpha:
                    break
            storage[self.hash] = (min_eval, moves)
            return min_eval, moves
    
    def alphabet(self, depth):
        _val, moves = self.alpha_beta_pruning(-math.inf, math.inf, depth, True)
        # print(moves)
        global storage
        print(storage)
        for move in moves:
            self.board.push(move)
        return _val, moves

    def __str__(self):
        return self.board

    def __repr__(self):
        return self.board
    
if __name__ == '__main__':
    engine = Engine('4r1rk/5K1b/7R/R7/8/8/8/8 w - - 0 1')
    print(engine.alphabet(3))