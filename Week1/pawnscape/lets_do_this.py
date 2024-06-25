import copy
import sys

# sys.setrecursionlimit(10**6)

class Board:
    def __init__(self):
        self.board = [
            ['b', 'b', 'b', 'b'],
            ['.', '.', '.', '.'],
            ['.', '.', '.', '.'],
            ['w', 'w', 'w', 'w']
        ]
    
    def __str__(self):
        out = '  0 1 2 3\n'
        for i in range(4):
            out += f'{i} '
            for j in range(4):
                out += f'{self.board[i][j]} '
            out += '\n'
        return out

    def legal_moves(self, player):
        if player == 'w':
            for i in range(4):
                for j in range(4):
                    if self.board[i][j] == 'w':
                        if i - 1 >= 0 and self.board[i-1][j] == '.':
                            yield f'{i-1}{j}{i}{j}'
                        if i - 1 >= 0 and j + 1 < 4 and self.board[i-1][j+1] == 'b':
                            yield f'{i-1}{j+1}{i}{j}'
                        if i - 1 >= 0 and j - 1 >= 0 and self.board[i-1][j-1] == 'b':
                            yield f'{i-1}{j-1}{i}{j}'
        if player == 'b':
            for i in range(4):
                for j in range(4):
                    if self.board[i][j] == 'b':
                        if i + 1 < 4 and self.board[i+1][j] == '.':
                            yield f'{i+1}{j}{i}{j}'
                        if i + 1 < 4 and j - 1 >= 0 and self.board[i+1][j-1] == 'w':
                            yield f'{i+1}{j-1}{i}{j}'
                        if i + 1 < 4 and j + 1 < 4 and self.board[i+1][j+1] == 'w':
                            yield f'{i+1}{j+1}{i}{j}'

    def check_win(self):
        if 'w' in self.board[0]:
            return 'w'
        if 'b' in self.board[3]:
            return 'b'
        if len(list(self.legal_moves('w'))) == 0 and len(list(self.legal_moves('b'))) == 0:
            return 'draw'
        return None

    def make_move(self, move):
        i1, j1, i2, j2 = int(move[0]), int(move[1]), int(move[2]), int(move[3])
        self.board[i1][j1] = self.board[i2][j2]
        self.board[i2][j2] = '.'

class State:
    def __init__(self, board, player):
        self.board = board
        self.player = player
        self.value = None
        self.is_terminal()

    def __str__(self):
        return self.board.__str__()
    
    def is_terminal(self):
        if self.board.check_win() is not None:
            if self.board.check_win() == 'w':
                self.value = 1
            elif self.board.check_win() == 'b':
                self.value = -1
            else:
                self.value = 0
            return True
        return False
    
    def get_children(self):
        if self.is_terminal():
            return []
        # return [State(copy.deepcopy(self.board), 'w' if self.player == 'b' else 'b') for move in self.board.legal_moves(self.player)]
        for move in self.board.legal_moves(self.player):
            board_copy = copy.deepcopy(self.board)
            board_copy.make_move(move)
            yield State(board_copy, 'w' if self.player == 'b' else 'b')
    
    def set_value(self):
        if self.is_terminal():
            return
        if self.player == 'w':
            self.value = max([state.set_value() for child in self.board.legal_moves(self.player)])
        else:
            self.value = min([State(self.board, 'w').set_value() for move in self.board.legal_moves(self.player)])
        return
    
class Stack:
    def __init__(self):
        self.stack = []
    
    def push(self, item):
        self.stack.append(item)
    
    def pop(self):
        return self.stack.pop()
    
    def is_empty(self):
        return len(self.stack) == 0

board = Board()
board.board = [
    ['.', 'b', 'b', 'b'],
    ['b', '.', '.', 'w'],
    ['.', '.', '.', '.'],
    ['w', 'w', 'w', '.']
]

state = State(board, 'b')
print(state.value)

# states_found = []
# states_needed_to_find = [state]

# for state in states_needed_to_find:
#     if state.value == None:
#         for child in state.get_children():
#             states_needed_to_find.append(child)
#     else:
#         states_found.append(state)

def minimax(state, depth, maximizing_player):
    if depth == 0 or state.value is not None:
        return state.value if state.value is not None else float('-inf') if maximizing_player else float('inf')

    if maximizing_player:
        max_eval = float('-inf')
        for child in state.get_children():
            eval = minimax(child, depth - 1, False)
            max_eval = max(max_eval, eval)
        return max_eval

    else:
        min_eval = float('inf')
        for child in state.get_children():
            eval = minimax(child, depth - 1, True)
            min_eval = min(min_eval, eval)
        return min_eval

best_value = float('-inf')
best_move = None
for move in state.board.legal_moves(state.player):
    board_copy = copy.deepcopy(state.board)
    board_copy.make_move(move)
    new_state = State(board_copy, 'w' if state.player == 'b' else 'b')
    value = minimax(new_state, 3, True)  # Change the depth as needed
    if value > best_value:
        best_value = value
        best_move = move

print(f"The best move is {best_move} with a value of {best_value}")

# board = Board()
# stack = Stack()
# states = []
# stack.push(State(board, 'w'))

# while not stack.is_empty():
#     state = stack.pop()
#     print(state.board)
#     # print()
#     if state.is_terminal():
#         print(state.value)
#         continue
#     for move in state.board.legal_moves(state.player):
#         board_copy = copy.deepcopy(state.board)
#         board_copy.make_move(move)
#         stack.push(State(board_copy, 'w' if state.player == 'b' else 'b'))
#         # print(stack.stack)
#         break

