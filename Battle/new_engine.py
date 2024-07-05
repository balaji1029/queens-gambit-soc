import chess
import chess.svg
import math
import sys
import numpy as np
import copy
import json
import time

class Engine:
    try:
        pieces: np.ndarray = np.load('pieces.npy')
        pawns_util: np.ndarray = np.load('pawns_util.npy')
        pawns_end_util: np.ndarray = np.load('pawns_end_util.npy')
        rooks_util: np.ndarray = np.load('rooks_util.npy')
        knights_util: np.ndarray = np.load('knights_util.npy')
        bishops_util: np.ndarray = np.load('bishops_util.npy')
        queens_util: np.ndarray = np.load('queens_util.npy')
        king_start_util: np.ndarray = np.load('king_start_util.npy')
        king_end_util: np.ndarray = np.load('king_end_util.npy')
    except:
        try:
            pieces: np.ndarray = np.load('/'.join(sys.argv[-1].split('/')[:-1]) + '/pieces.npy')
            pawns_util: np.ndarray = np.load('/'.join(sys.argv[-1].split('/')[:-1]) + '/pawns_util.npy')
            pawns_end_util: np.ndarray = np.load('/'.join(sys.argv[-1].split('/')[:-1]) + '/pawns_end_util.npy')
            rooks_util: np.ndarray = np.load('/'.join(sys.argv[-1].split('/')[:-1]) + '/rooks_util.npy')
            knights_util: np.ndarray = np.load('/'.join(sys.argv[-1].split('/')[:-1]) + '/knights_util.npy')
            bishops_util: np.ndarray = np.load('/'.join(sys.argv[-1].split('/')[:-1]) + '/bishops_util.npy')
            queens_util: np.ndarray = np.load('/'.join(sys.argv[-1].split('/')[:-1]) + '/queens_util.npy')
            king_start_util: np.ndarray = np.load('/'.join(sys.argv[-1].split('/')[:-1]) + '/king_start_util.npy')
            king_end_util: np.ndarray = np.load('/'.join(sys.argv[-1].split('/')[:-1]) + '/king_end_util.npy')
        except:
            print('pieces.npy not found.')
            sys.exit(1)

    def __init__(self, fen='rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq'):
        self.board = chess.Board(fen)
        self.hash = self.get_hash()
        # table = open('transposition_table.json', 'r')
        # self.transposition_table = json.loads(table.read())
        # self.storage = json.load(open('storage.json', 'r'))
        self.transposition_table = dict()
        self.storage = dict()

    def get_child(self, move: chess.Move) -> 'Engine':
        new = copy.deepcopy(self)
        new.make_move(move)
        return new

    def get_legal_moves(self) -> list:
        return list(self.board.legal_moves)
    
    @staticmethod
    def piece_map(piece: chess.Piece) -> int:
        piece_map = {'p': 0, 'n': 1, 'b': 2, 'r': 3, 'q': 4, 'k': 5, 'P': 6, 'N': 7, 'B': 8, 'R': 9, 'Q': 10, 'K': 11}
        return piece_map[str(piece)]
    
    def make_move(self, move: chess.Move) -> int:
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
        hash <<= 3
        if self.board.ep_square is not None:
            hash += int(self.board.ep_square % 8)
        hash <<= 1
        if self.board.has_kingside_castling_rights(chess.WHITE):
            hash ^= 1
        hash <<= 1
        if self.board.has_queenside_castling_rights(chess.WHITE):
            hash ^= 1
        hash <<= 1
        if self.board.has_kingside_castling_rights(chess.BLACK):
            hash ^= 1
        hash <<= 1
        if self.board.has_queenside_castling_rights(chess.BLACK):
            hash ^= 1
        hash <<= 1
        if self.board.turn == chess.WHITE:
            hash ^= 1
        self.hash = hash
        return hash
    
    def undo_move(self) -> int:
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
        hash <<= 3
        if self.board.ep_square is not None:
            hash += int(self.board.ep_square % 8)
        hash <<= 1
        if self.board.has_kingside_castling_rights(chess.WHITE):
            hash ^= 1
        hash <<= 1
        if self.board.has_queenside_castling_rights(chess.WHITE):
            hash ^= 1
        hash <<= 1
        if self.board.has_kingside_castling_rights(chess.BLACK):
            hash ^= 1
        hash <<= 1
        if self.board.has_queenside_castling_rights(chess.BLACK):
            hash ^= 1
        hash <<= 1
        if self.board.turn == chess.WHITE:
            hash ^= 1
        self.board.pop()
        self.hash = hash
        return hash

    def get_hash(self) -> int:
        hash = 0
        piece_map = {'p': 0, 'n': 1, 'b': 2, 'r': 3, 'q': 4, 'k': 5, 'P': 6, 'N': 7, 'B': 8, 'R': 9, 'Q': 10, 'K': 11}
        pieces = self.board.piece_map()
        for square, piece in pieces.items():
            hash ^= int(self.pieces[square][piece_map[str(piece)]])
        hash <<= 3
        if self.board.ep_square is not None:
            hash += int(self.board.ep_square % 8)
        hash <<= 1
        if self.board.has_kingside_castling_rights(chess.WHITE):
            hash ^= 1
        hash <<= 1
        if self.board.has_queenside_castling_rights(chess.WHITE):
            hash ^= 1
        hash <<= 1
        if self.board.has_kingside_castling_rights(chess.BLACK):
            hash ^= 1
        hash <<= 1
        if self.board.has_queenside_castling_rights(chess.BLACK):
            hash ^= 1
        hash <<= 1
        if self.board.turn == chess.WHITE:
            hash ^= 1
        return hash
    
    def eval(self) -> float:
        fen = self.hash >> 8
        if fen in self.storage:
            return self.storage[fen]
        white_pawns, black_pawns = 0, 0
        white_pieces, black_pieces = 0, 0
        white_bishop_count, black_bishop_count = 0, 0
        white_eval, black_eval = 0.0, 0.0
        map = self.board.piece_map()
        for square, piece in map.items():
            if piece is not None:
                piece_type = str(piece)
                index = self.piece_map(piece)
                if piece_type.isupper():
                    white_pieces += 1
                    if piece_type == 'P':
                        white_eval += 1.0
                        white_eval += self.pawns_util[square] / 100
                        white_pawns += 1
                    elif piece_type == 'N':
                        white_eval += 3.0
                        white_eval += self.knights_util[square] / 100
                    elif piece_type == 'B':
                        white_eval += 3.25
                        white_eval += self.bishops_util[square] / 100
                        white_bishop_count += 1
                    elif piece_type == 'R':
                        white_eval += 5.0
                        white_eval += self.rooks_util[square] / 100
                    elif piece_type == 'Q':
                        white_eval += 9.75
                        white_eval += self.queens_util[square] / 100
                    elif piece_type == 'K':
                        if (white_pieces + black_pieces) > 8:
                            white_eval += self.king_start_util[square] / 100
                        else:
                            white_eval += self.king_end_util[square] / 100
                else:
                    black_pieces += 1
                    if piece_type == 'p':
                        black_eval += 1.0
                        black_eval += self.pawns_util[chess.square_mirror(square)] / 100
                        black_pawns += 1
                    elif piece_type == 'n':
                        black_eval += 3.0
                        black_eval += self.knights_util[chess.square_mirror(square)] / 100
                    elif piece_type == 'b':
                        black_eval += 3.25
                        black_eval += self.bishops_util[chess.square_mirror(square)] / 100
                        black_bishop_count += 1
                    elif piece_type == 'r':
                        black_eval += 5.0
                        black_eval += self.rooks_util[chess.square_mirror(square)] / 100
                    elif piece_type == 'q':
                        black_eval += 9.75
                        black_eval += self.queens_util[chess.square_mirror(square)] / 100
                    elif piece_type == 'k':
                        if (white_pieces + black_pieces) > 8:
                            black_eval += self.king_start_util[chess.square_mirror(square)] / 100
                        else:
                            black_eval += self.king_end_util[chess.square_mirror(square)] / 100
        if white_bishop_count >= 2:
            white_eval += 0.5
        if black_bishop_count >= 2:
            black_eval += 0.5
        score = white_eval - black_eval
        self.storage[fen] = score
        return score

    def move_order(self, moves: list) -> list:
        move_list = []
        for move in moves:
            self.board.push(move)
            if self.board.is_checkmate():
                self.board.pop()
                return [move]
            elif self.board.is_check():
                move_list.append((50, move))
            else:
                move_list.append((self.eval(), move))
            self.board.pop()
        move_list = sorted(move_list, key=lambda x: x[0], reverse=self.board.turn)
        return [move for _, move in move_list]

    def alpha_beta_pruning(self, depth: int, alpha: float, beta: float) -> tuple[float, str]:
        if self.hash in self.transposition_table and self.transposition_table[self.hash][0] >= depth:
            return self.transposition_table[self.hash][1], chess.Move.from_uci(self.transposition_table[self.hash][2])
        if depth == 0:
            return self.eval(), ''
        legal_moves = self.get_legal_moves()
        if not legal_moves:
            return (-math.inf if self.board.turn else math.inf), ''
        best_move = legal_moves[0]
        ordered_moves = self.move_order(legal_moves)
        for move in ordered_moves:
            self.make_move(move)
            if self.hash in self.transposition_table and self.transposition_table[self.hash][0] >= depth:
                score = self.transposition_table[self.hash][1]
            else:
                score, _ = self.alpha_beta_pruning(depth - 1, alpha, beta)
            self.undo_move()
            if self.board.turn:
                if score > alpha:
                    alpha = score
                    best_move = move
                if alpha >= beta:
                    break
            else:
                if score < beta:
                    beta = score
                    best_move = move
                if alpha >= beta:
                    break
        self.transposition_table[self.hash] = (depth, alpha if self.board.turn else beta, str(best_move))
        return alpha if self.board.turn else beta, best_move

    def iterative_deepening(self, max_depth: int) -> str:
        best_move = ''
        for depth in range(1, max_depth + 1):
            _, best_move = self.alpha_beta_pruning(depth, -math.inf, math.inf)
        return best_move

    def get_best_move(self, max_depth: int = 3) -> str:
        start = time.time()
        move = self.alpha_beta_pruning(max_depth, -math.inf, math.inf)[1]
        print(f"Time taken: {time.time() - start}")
        return move

# Initialize the engine and get the best move
engine = Engine()
# start = time.time()
Movenumber=1
pgnstr=""
# display(engine.board)
whitetime=[]
blacktime=[]
while not engine.board.is_game_over():
    start=time.time()
    move=engine.get_best_move(5)
    t1=time.time()-start
    whitetime.append(t1)
    sanmove=engine.board.san(move)
    pgnstr+=str(Movenumber)+". "+str(sanmove)+" "
    engine.make_move(move)
    print(engine.board)
#     print(pgnstr)
    print("Time taken for last move:",t1)
    if engine.board.is_game_over():
        break
    start=time.time()
    move=engine.get_best_move(5)
    t2=time.time()-start
    blacktime.append(t2)
    sanmove=engine.board.san(move)
    pgnstr+=str(Movenumber)+". "+str(sanmove)+" "
    engine.make_move(move)
    print(engine.board)
#     print(pgnstr)
    print("Time taken for last move:",t2)
    Movenumber+=1
print(pgnstr)
print("White time:",sum(whitetime))
print("Black time:",sum(blacktime))
json.dump(engine.storage, open('storage.json', 'w'))
json.dump(engine.transposition_table, open('transposition_table.json', 'w'))
# print(f"Time taken: {time.time() - start}")