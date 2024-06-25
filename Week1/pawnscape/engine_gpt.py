import copy

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
                            moves.append(f'{i-1}{j}{i}{j}')
                        if i - 1 >= 0 and j + 1 < 4 and self.board[i-1][j+1] == 'b':
                            moves.append(f'{i-1}{j+1}{i}{j}')
                        if i - 1 >= 0 and j - 1 >= 0 and self.board[i-1][j-1] == 'b':
                            moves.append(f'{i-1}{j-1}{i}{j}')
                    else:
                        if i + 1 < 4 and self.board[i+1][j] == '.':
                            moves.append(f'{i+1}{j}{i}{j}')
                        if i + 1 < 4 and j - 1 >= 0 and self.board[i+1][j-1] == 'w':
                            moves.append(f'{i+1}{j-1}{i}{j}')
                        if i + 1 < 4 and j + 1 < 4 and self.board[i+1][j+1] == 'w':
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
        self.board[i2][j2] = self.board[i1][j1]
        self.board[i1][j1] = '.'
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
            board.board = copy.deepcopy(self.board)
            board.turn = self.turn
            board.make_move(move)
            boards.append(board)
        return boards

class Engine:
    def __init__(self):
        self.board = Board()
        self.move_history = []
        self.stack = Stack()
        self.stack.push(State(self.board, 'w', []))
        self.states = []
        self.winning_histories = []

    def go_to_state(self, state):
        for i in range(4):
            self.board.board[i] = copy.deepcopy(state.board.board[i])
        self.board.turn = state.turn
        self.move_history = copy.deepcopy(state.move_history)

    def run(self):
        while self.round() == 0:
            continue

    def get_possible_states(self):
        states = []
        for move in self.get_legal_moves():
            new_board = Board()
            new_board.board = copy.deepcopy(self.board.board)
            new_board.turn = self.board.turn
            new_board.make_move(move)
            states.append(State(new_board, 'b' if self.board.turn == 'w' else 'w', self.move_history + [move]))
        return states

    def round(self):
        self.print_board()
        print(f'{self.board.turn.upper()}\'s turn\n')
        move = self.take_input()
        self.make_move(move)
        if self.board.check_win() is not None:
            self.print_board()
            print(f'{self.board.check_win().upper()} won!')
            return 1
        self.board.turn = 'b' if self.board.turn == 'w' else 'w'
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

    def winning_strategy_for_white(self):
        return self.back_track_strategy()

    def back_track_strategy(self):
        while not self.stack.is_empty():
            state = self.stack.pop()
            if state in self.states:
                continue
            self.states.append(state)
            self.go_to_state(state)
            if self.board.check_win() is not None:
                if self.board.check_win() == 'w':
                    self.winning_histories.append(self.move_history.copy())
            else:
                child_states = self.get_possible_states()
                if all(self.is_losing_state_for_black(child) for child in child_states):
                    self.stack.push(state)
                    self.winning_histories.append(self.move_history.copy())
                else:
                    for child_state in child_states:
                        self.stack.push(child_state)
        if self.winning_histories:
            for history in self.winning_histories:
                print('Winning move sequence:', history)
        else:
            print('No winning moves found')

    def is_losing_state_for_black(self, state):
        self.go_to_state(state)
        if self.board.check_win() == 'b':
            return True
        possible_states = self.get_possible_states()
        return all(self.is_winning_state_for_white(child) for child in possible_states)

    def is_winning_state_for_white(self, state):
        self.go_to_state(state)
        if self.board.check_win() == 'w':
            return True
        possible_states = self.get_possible_states()
        return any(self.is_losing_state_for_black(child) for child in possible_states)

    def print_board(self):
        print()
        print(self.board)
        print()

if __name__ == '__main__':
    engine = Engine()
    engine.winning_strategy_for_white()
    # engine.run()
