import chess
import chess.svg
import math
import sys
import numpy as np
import copy
import time

storage = dict()
moves = dict()

class Engine:
    """A class to represent a chess engine written by Balaji."""
    try:
        pieces: np.ndarray = np.load('pieces.npy')
    except:
        try:
            pieces: np.ndarray = np.load('/'.join(sys.argv[-1].split('/')[:-1]) + '/pieces.npy')
        except:
            print('pieces.npy not found.')
            sys.exit(1)

    def __init__(self, fen='rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq'):
        """Initializes the board with the given FEN string. If no FEN string is given, initializes the board with the default starting position."""
        self.board = chess.Board(fen)
        self.hash = self.get_hash()

    def set_fen(self, fen: str) -> None:
        """Sets the board position to the given FEN string."""
        self.board.set_fen(fen)
        self.hash = self.get_hash()

    def get_child(self, move: chess.Move) -> 'Engine':
        """Returns a new board with the move executed on the current board."""
        new = copy.deepcopy(self)
        new.board.push(move)
        return new

    def get_legal_moves(self) -> list:
        """Returns a list of legal moves from the current board."""
        return list(self.board.legal_moves)
    
    @staticmethod
    def piece_map(piece: chess.Piece) -> int:
        """Returns the index of the piece in the pieces array."""
        piece_map = {'p': 0, 'n': 1, 'b': 2, 'r': 3, 'q': 4, 'k': 5, 'P': 6, 'N': 7, 'B': 8, 'R': 9, 'Q': 10, 'K': 11}
        # print(piece, str(piece))
        return piece_map[str(piece)]
    
    def make_move(self, move: chess.Move) -> int:
        """Makes the move on the board and updates the hash."""
        piece_map = {'p': 0, 'n': 1, 'b': 2, 'r': 3, 'q': 4, 'k': 5, 'P': 6, 'N': 7, 'B': 8, 'R': 9, 'Q': 10, 'K': 11}
        hash = self.hash >> 8
        moving = str(self.board.piece_at(move.from_square))
        moving_piece = piece_map[moving]
        if self.board.is_castling(move):
            if move.uci() == 'e1g1' and moving == 'K':
                hash ^= int(self.pieces[chess.E1][moving_piece])
                hash ^= int(self.pieces[chess.G1][moving_piece])
                hash ^= int(self.pieces[chess.H1][piece_map['R']])
                hash ^= int(self.pieces[chess.F1][piece_map['R']])
            elif move.uci() == 'e1c1' and moving == 'K':
                hash ^= int(self.pieces[chess.E1][moving_piece])
                hash ^= int(self.pieces[chess.C1][moving_piece])
                hash ^= int(self.pieces[chess.A1][piece_map['R']])
                hash ^= int(self.pieces[chess.D1][piece_map['R']])
            elif move.uci() == 'e8g8' and moving == 'k':
                hash ^= int(self.pieces[chess.E8][moving_piece])
                hash ^= int(self.pieces[chess.G8][moving_piece])
                hash ^= int(self.pieces[chess.H8][piece_map['r']])
                hash ^= int(self.pieces[chess.F8][piece_map['r']])
            elif move.uci() == 'e8c8' and moving == 'k':
                hash ^= int(self.pieces[chess.E8][moving_piece])
                hash ^= int(self.pieces[chess.C8][moving_piece])
                hash ^= int(self.pieces[chess.A8][piece_map['r']])
                hash ^= int(self.pieces[chess.D8][piece_map['r']])
        elif self.board.is_en_passant(move):
            hash ^= int(self.pieces[move.from_square][moving_piece])
            hash ^= int(self.pieces[move.to_square][moving_piece])
            square = int(chess.square(chess.square_file(move.to_square), chess.square_rank(move.from_square)))
            hash ^= int(self.pieces[square][piece_map[str(self.board.piece_at(square))]])
        elif move.promotion is not None:
            hash ^= int(self.pieces[move.from_square][moving_piece])
            promotion_piece = str(chess.Piece(move.promotion, self.board.turn))
            hash ^= int(self.pieces[move.to_square][piece_map[promotion_piece]])
            if self.board.is_capture(move):
                hash ^= int(self.pieces[move.to_square][piece_map[str(self.board.piece_at(move.to_square))]])
        else:
            hash ^= int(self.pieces[move.from_square][moving_piece])
            hash ^= int(self.pieces[move.to_square][moving_piece])
            if self.board.is_capture(move):
                hash ^= int(self.pieces[move.to_square][piece_map[str(self.board.piece_at(move.to_square))]])
        self.board.push(move)
        # Update hash for en passant
        # hash <<= 3
        # if self.board.ep_square is not None:
        #     hash += int(self.board.ep_square % 8)
        # # Castling rights
        # hash <<= 1
        # if self.board.has_kingside_castling_rights(chess.WHITE):
        #     hash ^= 1
        # hash <<= 1
        # if self.board.has_queenside_castling_rights(chess.WHITE):
        #     hash ^= 1
        # hash <<= 1
        # if self.board.has_kingside_castling_rights(chess.BLACK):
        #     hash ^= 1
        # hash <<= 1
        # if self.board.has_queenside_castling_rights(chess.BLACK):
        #     hash ^= 1
        # # Turn
        # hash <<= 1
        # if self.board.turn == chess.WHITE:
        #     hash ^= 1
        # self.hash = hash
        # self.hash = self.get_hash()
        fen = self.board.fen().split(' ')
        castling = fen[2]
        turn = fen[1]
        # En passant
        en_passant = fen[3]
        hash <<= 3
        if en_passant != '-':
            hash += int(ord(en_passant[0]) - ord('a'))
        # ep_square = board1.ep_square
        # if ep_square is not None:
        #     hash += int(ep_square % 8)
        # Castling rights
        hash <<= 1
        if 'K' in castling:
            hash ^= 1
        hash <<= 1
        if 'Q' in castling:
            hash ^= 1
        hash <<= 1
        if 'k' in castling:
            hash ^= 1
        hash <<= 1
        if 'q' in castling:
            hash ^= 1
        # Turn
        hash <<= 1
        if turn == 'w':
            hash ^= 1
        self.hash = hash
        return hash
    
    def undo_move(self) -> int:
        """Undoes the last move and updates the hash."""
        piece_map = {'p': 0, 'n': 1, 'b': 2, 'r': 3, 'q': 4, 'k': 5, 'P': 6, 'N': 7, 'B': 8, 'R': 9, 'Q': 10, 'K': 11}
        hash = self.hash >> 8
        board1 = self.board.copy()
        move = board1.pop()
        moving = board1.piece_at(move.from_square)
        moving_piece = piece_map[str(moving)]
        if self.board.is_castling(move):
            if move.uci() == 'e1g1' and moving == 'K':
                hash ^= int(self.pieces[chess.E1][moving_piece])
                hash ^= int(self.pieces[chess.G1][moving_piece])
                hash ^= int(self.pieces[chess.H1][piece_map['R']])
                hash ^= int(self.pieces[chess.F1][piece_map['R']])
            elif move.uci() == 'e1c1' and moving == 'K':
                hash ^= int(self.pieces[chess.E1][moving_piece])
                hash ^= int(self.pieces[chess.C1][moving_piece])
                hash ^= int(self.pieces[chess.A1][piece_map['R']])
                hash ^= int(self.pieces[chess.D1][piece_map['R']])
            elif move.uci() == 'e8g8' and moving == 'k':
                hash ^= int(self.pieces[chess.E8][moving_piece])
                hash ^= int(self.pieces[chess.G8][moving_piece])
                hash ^= int(self.pieces[chess.H8][piece_map['r']])
                hash ^= int(self.pieces[chess.F8][piece_map['r']])
            elif move.uci() == 'e8c8' and moving == 'k':
                hash ^= int(self.pieces[chess.E8][moving_piece])
                hash ^= int(self.pieces[chess.C8][moving_piece])
                hash ^= int(self.pieces[chess.A8][piece_map['r']])
                hash ^= int(self.pieces[chess.D8][piece_map['r']])
            
        elif board1.is_en_passant(move):
            hash ^= int(self.pieces[move.from_square][moving_piece])
            hash ^= int(self.pieces[move.to_square][moving_piece])
            square = int(chess.square(chess.square_file(move.to_square), chess.square_rank(move.from_square)))
            hash ^= int(self.pieces[square][piece_map[str(board1.piece_at(square))]])
        elif move.promotion is not None:
            hash ^= int(self.pieces[move.from_square][moving_piece])
            promotion_piece = str(chess.Piece(move.promotion, board1.turn))
            hash ^= int(self.pieces[move.to_square][piece_map[promotion_piece]])
            if board1.is_capture(move):
                hash ^= int(self.pieces[move.to_square][piece_map[str(board1.piece_at(move.to_square))]])
        else:
            hash ^= int(self.pieces[move.from_square][moving_piece])
            hash ^= int(self.pieces[move.to_square][moving_piece])
            if board1.is_capture(move):
                hash ^= int(self.pieces[move.to_square][piece_map[str(board1.piece_at(move.to_square))]])            
        self.board.pop()
        # Update hash for en passant
        # hash <<= 3
        # if board1.ep_square is not None:
        #     hash += int(board1.ep_square % 8)
        # # Castling rights
        # hash <<= 1
        # if board1.has_kingside_castling_rights(chess.WHITE):
        #     hash ^= 1
        # hash <<= 1
        # if board1.has_queenside_castling_rights(chess.WHITE):
        #     hash ^= 1
        # hash <<= 1
        # if board1.has_kingside_castling_rights(chess.BLACK):
        #     hash ^= 1
        # hash <<= 1
        # if board1.has_queenside_castling_rights(chess.BLACK):
        #     hash ^= 1
        # # Turn
        # hash <<= 1
        # if board1.turn == chess.WHITE:
        #     hash ^= 1
        fen = board1.fen().split(' ')
        castling = fen[2]
        turn = fen[1]
        # En passant
        en_passant = fen[3]
        hash <<= 3
        if en_passant != '-':
            hash += int(ord(en_passant[0]) - ord('a'))
        # ep_square = board1.ep_square
        # if ep_square is not None:
        #     hash += int(ep_square % 8)
        # Castling rights
        hash <<= 1
        if 'K' in castling:
            hash ^= 1
        hash <<= 1
        if 'Q' in castling:
            hash ^= 1
        hash <<= 1
        if 'k' in castling:
            hash ^= 1
        hash <<= 1
        if 'q' in castling:
            hash ^= 1
        # Turn
        hash <<= 1
        if turn == 'w':
            hash ^= 1
        self.hash = hash
        return hash
    
    # Zobrist Hashing
    def get_hash(self) -> int:
        """Returns the Zobrist hash of the current board position."""
        piece_map = {'p': 0, 'n': 1, 'b': 2, 'r': 3, 'q': 4, 'k': 5, 'P': 6, 'N': 7, 'B': 8, 'R': 9, 'Q': 10, 'K': 11}
        hash = 0
        map = self.board.piece_map()
        pieces = self.pieces
        for square, piece in map.items():
            hash ^= int(pieces[square][piece_map[str(piece)]])
        # Castling info
        fen = self.board.fen().split(' ')
        castling = fen[2]
        turn = fen[1]
        # En passant
        en_passant = fen[3]
        hash <<= 3
        if en_passant != '-':
            hash += int(ord(en_passant[0]) - ord('a'))
        # ep_square = self.board.ep_square
        # if ep_square is not None:
        #     hash += int(ep_square % 8)
        # Castling rights
        hash <<= 1
        if 'K' in castling:
            hash ^= 1
        hash <<= 1
        if 'Q' in castling:
            hash ^= 1
        hash <<= 1
        if 'k' in castling:
            hash ^= 1
        hash <<= 1
        if 'q' in castling:
            hash ^= 1
        # Turn
        hash <<= 1
        if turn == 'w':
            hash ^= 1
        return hash
    
    def get_ordered_moves(self) -> list:
        """Returns a list of legal moves ordered according to the number of legal moves in the next state, then by check. If a move leads to a checkmate, it is returned immediately."""
        global moves
        moves_dict = dict()
        board = copy.deepcopy(self.board)
        for move in self.board.legal_moves:
            self.board.push(move)
            if board.is_checkmate():
                board.pop()
                return [move]
            moves_dict[move] = math.log(len(list(self.board.legal_moves))+1)*0.693
            if board.is_check():
                moves_dict[move] = -math.inf
            self.board.pop()
            # if board.is_capture(move):
            #     pass
            #     moves_dict[move] = -0.8
            # if self.board.piece_at(move.to_square) is not chess.PAWN:
            #     moves_dict[move] -= 1000
            # else:
            #     moves_dict[move] = 0
        return sorted(moves_dict, key=lambda x: moves_dict[x])

        # good_moves = []
        # capture_moves = []
        # other_moves = []
        
        
        
        # for move in self.board.legal_moves:
        #     sanmove = self.board.san(move)
        #     if sanmove[-1]=='#':
        #         return [move]
        #     if sanmove[-1] in {'+', 'Q'}:
        #         good_moves.append(move)
        #     elif sanmove[1] == 'x':
        #         capture_moves.append(move)
        #     else:
        #         other_moves.append(move)
    
        # # Combine all the categorized moves
        # valid_actions = good_moves + capture_moves + other_moves
        # return valid_actions
    
    def eval(self) -> int:
        """Returns the static evaluation of the current board position."""
        if self.board.is_checkmate():
            if self.board.turn:
                return -math.inf
            else:
                return math.inf
        if self.board.is_stalemate():
            return 0
        # return 0
        # map = self.board.piece_map()
        fen = self.board.fen().split(' ')[0]
        ref = {'p': -100, 'n': -320, 'b': -330, 'r': -500, 'q': -900, 'P': 100, 'N': 320, 'B': 330, 'R': 500, 'Q': 900, 'k': -20000, 'K': 20000}
        # pieces = ['q', 'r', 'b', 'n', 'p']
        score = 0
        for ch in fen:
            if ch.isalpha():
                score += ref[ch]
        # turn = self.board.turn
        # chess_map = self.board.piece_map()
        # for square, piece in chess_map.items():
        #     if piece == None:
        #         continue
        #     if piece.color == turn:
        #         score += ref[str(piece).lower()]
        # print(self.board.turn == chess.BLACK, max_player)
        # for piece in pieces:
        #     score += ((count[piece] if piece in count else 0) - (count[piece.upper()] if piece.upper() in count else 0)) * ref[piece]
        # print(-score if ((max_player and self.board.turn == chess.WHITE) or (not max_player and self.board.turn == chess.BLACK)) else score)
        # try:
        #     display(self.board)
        # except:
        #     pass
        return score

    def alpha_beta_pruning(self, alpha: float, beta: float, depth: int, max_player: bool) -> tuple:
        """Returns the evaluation of the current board position and the best move for the current player, using alpha-beta pruning with a depth of `depth`, and the player to maximize the evaluation is `max_player`."""
        global storage
        # print(self.hash)
        if depth == 0 or self.board.is_game_over():
            return self.eval(), None
        if max_player:
            max_eval = -math.inf
            best_move = None
            for move in self.get_ordered_moves():
                new = self.get_child(move)
                # if ((new.hash << 3) + depth-1) in storage:
                #     eval, _move = storage[new.hash]
                # else:
                eval, _move = new.alpha_beta_pruning(alpha, beta, depth - 1, not max_player)
                # if depth == 5:
                #     print(move, eval)
                if eval >= max_eval:
                    best_move = move
                max_eval = max(max_eval, eval)
                alpha = max(alpha, eval)
                if beta <= alpha:
                    break
            # storage[self.hash << 3 + depth] = (max_eval, best_move)
            return max_eval, best_move
        else:
            min_eval = math.inf
            best_move = None
            for move in self.get_ordered_moves():
                new = self.get_child(move)
                # if ((new.hash << 3) + depth-1) in storage:
                #     eval, _move = storage[new.hash]
                # else:
                eval, _move = new.alpha_beta_pruning(alpha, beta, depth - 1, not max_player)
                # if depth == 5:
                #     print(move, eval)
                if eval <= min_eval:
                    best_move = move
                min_eval = min(min_eval, eval)
                beta = min(beta, eval)
                if beta <= alpha:
                    break
            # storage[self.hash << 3 + depth] = (min_eval, best_move)
            return min_eval, best_move
    
    def alphabet(self: 'Engine', depth: int) -> None:
        """Makes the best move for the current player using alpha-beta pruning until the depth of `depth`."""
        global storage
        start = time.time()
        # Iterative Deepening
        _val, move = self.alpha_beta_pruning(-math.inf, math.inf, depth, self.board.turn)
        for i in range(depth):
            self.board.push(move)
            if self.board.is_game_over():
                return
            _val, move = self.alpha_beta_pruning(-math.inf, math.inf, depth-i, self.board.turn)
            # _val, move = storage[self.hash << 3 + depth-i]
    
    def get_move(self, depth: int):
        """Returns the best move for the current player using alpha-beta pruning with a depth of `depth`."""
        _val, move = self.alpha_beta_pruning(-math.inf, math.inf, depth, self.board.turn)
        return _val, move

    def __str__(self) -> str:
        """Returns the string representation of the board."""
        return self.board.__str__()

    def __repr__(self) -> str:
        """Returns the string representation of the board."""
        return self.board.__repr__()
    
if __name__ == '__main__':
    print('This is a chess engine written by Balaji. And it is not meant to be run directly.')