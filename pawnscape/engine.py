import copy
import json

class State:
    def __init__(self, board, turn, move_history):
        self.board = board
        self.turn = turn
        self.move_history = move_history

    def __str__(self) -> str:
        return f'{self.board}\n{self.turn}\n{self.move_history}'

    def __eq__(self, o: object) -> bool:
        return self.board == o.board and self.turn == o.turn and self.move_history == o.move_history

class Stack:
    def __init__(self):
        self.stack = []

    def push(self, state):
        self.stack.append(state)

    def pop(self):
        return self.stack.pop()

    def is_empty(self):
        return len(self.stack) == 0

    def __str__(self) -> str:
        return '\n'.join([str(state) for state in self.stack])

class Board:
    def __init__(self):
        self.board = [
            ['b', 'b', 'b', 'b'],
            ['.', '.', '.', '.'],
            ['.', '.', '.', '.'],
            ['w', 'w', 'w', 'w']
        ]
        self.turn = 'w'

    def possible_moves(self):
        moves = []
        for i in range(4):
            for j in range(4):
                if self.board[i][j] == self.turn:
                    if self.turn == 'w':
                        if i - 1 >= 0 and self.board[i-1][j] == '.':
                            # moves.append((i, j, i-1, j))
                            moves.append(f'{i-1}{j}{i}{j}')
                        if i - 1 >= 0 and j + 1 < 4 and self.board[i-1][j+1] == 'b':
                            # moves.append((i, j, i-1, j+1))
                            moves.append(f'{i-1}{j+1}{i}{j}')
                        if i - 1 >= 0 and j - 1 >= 0 and self.board[i-1][j-1] == 'b':
                            # moves.append((i, j, i-1, j-1))
                            moves.append(f'{i-1}{j-1}{i}{j}')
                    else:
                        if i + 1 < 4 and self.board[i+1][j] == '.':
                            # moves.append((i, j, i+1, j))
                            moves.append(f'{i+1}{j}{i}{j}')
                        if i + 1 < 4 and j - 1 >= 0 and self.board[i+1][j-1] == 'w':
                            # moves.append((i, j, i+1, j-1))
                            moves.append(f'{i+1}{j-1}{i}{j}')
                        if i + 1 < 4 and j + 1 < 4 and self.board[i+1][j+1] == 'w':
                            # moves.append((i, j, i+1, j+1))
                            moves.append(f'{i+1}{j+1}{i}{j}')
        return moves
    
    def check_win(self):
        if 'w' in self.board[0]:
            return 'w'
        if 'b' in self.board[3]:
            return 'b'
        if len(self.possible_moves()) == 0:
            return 'draw'
        return None
    
    def make_move(self, move):
        i1, j1, i2, j2 = int(move[0]), int(move[1]), int(move[2]), int(move[3])
        self.board[i1][j1] = self.board[i2][j2]
        self.board[i2][j2] = '.'
        self.turn = 'w' if self.turn == 'b' else 'b'
    
    def undo_move(self, move):
        i1, j1, i2, j2 = int(move[0]), int(move[1]), int(move[2]), int(move[3])
        self.board[i1][j1] = self.board[i2][j2]
        self.board[i2][j2] = '.'
        self.turn = 'w' if self.turn == 'b' else 'b'

    def __str__(self) -> str:
        board = '  0 1 2 3\n'
        for i in range(4):
            board += f'{i} '
            for j in range(4):
                board += f'{self.board[i][j]} '
            board += '\n'
        return board.upper()
    
    def get_possible_boards(self):
        boards = []
        for move in self.possible_moves():
            board = Board()
            board.board = self.board.copy()
            board.turn = self.turn
            board.make_move(move)
            boards.append(board)
        return boards
    
    def get_children(self):
        children = []
        for move in self.possible_moves():
            board = Board()
            board.board = copy.deepcopy(self.board)
            board.turn = self.turn
            board.make_move(move)
            children.append(board)
        return children

class Engine:
    def __init__(self):
        self.board = Board()
        # self.move_history = []
        self.stack = Stack()
        self.stack.push(State(self.board, 'w', []))
        self.states = []
        self.move_history = []
        black_mdp_file = open('black-mdp.json', 'r')
        self.black_mdp = json.load(black_mdp_file)
        black_mdp_file.close()
        white_mdp_file = open('white-mdp.json', 'r')
        self.white_mdp = json.load(white_mdp_file)
        white_mdp_file.close()

    # def __init__(self, board, turn, move_history):
    #     self.board = board
    #     self.stack = Stack()
    #     self.stack.push(State(self.board, turn, move_history))
    #     self.states = []
    #     self.move_history = []
    #     black_mdp_file = open('black-mdp.json', 'r')
    #     self.black_mdp = json.load(black_mdp_file)
    #     black_mdp_file.close()
    #     white_mdp_file = open('white-mdp.json', 'r')
    #     self.white_mdp = json.load(white_mdp_file)
    #     white_mdp_file.close()

    def go_to_state(self, state):
        print(state)
        for i in range(len(self.board.board)):
            self.board.board[i] = copy.deepcopy(state.board.board[i])
        self.board.turn = state.turn
        self.move_history = copy.deepcopy(state.move_history)

    def run(self):
        while self.round() == 0:
            print()
            continue

    def run_with_strategy(self):
        while self.strat_round() == 0:
            print()
            continue

    def strat_round(self):
        self.print_board()
        print(f'{self.board.turn.upper()}\'s turn\n')
        if self.board.turn == 'w':
            move = self.get_white_input()
        else:
            move = self.get_black_input()
        self.make_move(move)
        if self.board.check_win() is not None:
            self.print_board()
            if self.board.check_win() == 'draw':
                print('It\'s a draw!')
            else:
                print(f'{self.board.check_win().upper()} won!')
            return 1
        self.print_board()
        print(f'{self.board.turn.upper()}\'s turn\n')
        # move = self.take_input()
        if self.board.turn == 'w':
            move = self.get_white_input()
        else:
            move = self.get_black_input()
        self.make_move(move)
        if self.board.check_win() is not None:
            self.print_board()
            if self.board.check_win() == 'draw':
                print('It\'s a draw!')
            else:
                print(f'{self.board.check_win().upper()} won!')
            return 1
        self.board.turn = 'w'
        return 0
    
    def get_white_input(self):
        board_copy = copy.deepcopy(self.board.board)
        move = self.get_move_value(board_copy, self.white_mdp[str(board_copy).upper()])
        print('Computer moved: ', move)
        return move
    
    def get_black_input(self):
        board_copy = copy.deepcopy(self.board.board)
        move = self.get_move_value(board_copy, self.black_mdp[str(board_copy).upper()])
        print('Computer moved: ', move)
        return move

    def get_move_value(self, before_move, after_move):
        move = '    '
        for i in range(4):
            for j in range(4):
                if before_move[i][j].upper() != after_move[i][j]:
                    if after_move[i][j] == '.':
                        move = f'{move[0]}{move[1]}{i}{j}'
                    else:
                        move = f'{i}{j}{move[2]}{move[3]}'
        return move

    def get_possible_states(self):
        states = []
        for move in self.get_legal_moves():
            board = Board()
            board.board = self.board.board.copy()
            board.turn = self.board.turn
            board.make_move(move)
            states.append(State(board, 'b' if self.board.turn == 'w' else 'w', self.move_history + [move]))
        return states

    def round(self):
        self.print_board()
        print(f'{self.board.turn.upper()}\'s turn\n')
        move = self.take_input()
        self.make_move(move)
        if self.board.check_win() is not None:
            self.print_board()
            if self.board.check_win() == 'draw':
                print('It\'s a draw!')
            else:
                print(f'{self.board.check_win().upper()} won!')
            return 1
        self.board.turn = 'b'
        self.print_board()
        print(f'{self.board.turn.upper()}\'s turn\n')
        move = self.take_input()
        self.make_move(move)
        if self.board.check_win() is not None:
            self.print_board()
            if self.board.check_win() == 'draw':
                print('It\'s a draw!')
            else:
                print(f'{self.board.check_win().upper()} won!')
            return 1
        self.board.turn = 'w'
        return 0

    def take_input(self):
        move = input('Enter the move: ')
        while move not in self.get_legal_moves():
            print('Invalid move')
            move = input('Enter the move: ')
        return move

    def make_move(self, move):
        self.move_history.append(move)
        self.board.make_move(move)

    def get_legal_moves(self):
        return self.board.possible_moves()

    def get_board(self):
        return self.board

    # def get_move_history(self):
    #     return self.move_history

    def back_track(self):
        print(self.stack)
        if self.stack.is_empty():
            return None
        state = self.stack.pop()
        while state in self.states:
            state = self.stack.pop()
        print(state)
        self.go_to_state(state)
        if self.board.check_win() is None:
            for child_state in self.get_possible_states():
                self.stack.push(child_state)
        else:
            print('Winning move: ', self.move_history)
            return self.move_history
        self.back_track()
    
    def print_board(self):
        print()
        print(self.board)
        print()

  
if __name__ == '__main__':
    engine = Engine()
    # engine.back_track()
    engine.run_with_strategy()
    board = Board()
    board.board = [
        ['b', 'b', '.', 'b'],
        ['.', '.', 'b', '.'],
        ['.', '.', 'w', '.'],
        ['w', 'w', '.', 'w']
    ]
    engine.board = board
    engine.run_with_strategy()