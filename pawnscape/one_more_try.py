import copy

class State:
    def __init__(self):
        self.board = [
            ['B', 'B', 'B', 'B'],
            ['.', '.', '.', '.'],
            ['.', '.', '.', '.'],
            ['W', 'W', 'W', 'W']
        ]
        self.player = 'W'
        self.value = None

    def __init__(self, board, player):
        self.board = board
        self.player = player
        if self.is_terminal():
            if self.is_terminal() == 'W':
                self.value = 1
            elif self.is_terminal() == 'B':
                self.value = -1
            else:
                self.value = 0
        else:
            if self.player == 'W':
                self.value = max([child.value for child in self.get_children()])
            else:
                self.value = min([child.value for child in self.get_children()])

    def get_children(self):
        for move in legal_moves(self.board, self.player):
            board_copy = make_move(self.board, move)
            yield State(board_copy, 'W' if self.player == 'B' else 'B')

    def is_terminal(self):
        if 'W' in self.board[0]:
            return 'W'
        if 'B' in self.board[3]:
            return 'B'
        if len(list(legal_moves(self.board, self.player))) == 0:
            return 'draw'
        return None
        

def legal_moves(board, player):
    for i in range(4):
        for j in range(4):
            if player == 'W':
                if board[i][j] == 'W':
                    if i - 1 >= 0 and board[i-1][j] == '.':
                        yield f'{i-1}{j}{i}{j}'
                    if i - 1 >= 0 and j + 1 < 4 and board[i-1][j+1] == 'B':
                        yield f'{i-1}{j+1}{i}{j}'
                    if i - 1 >= 0 and j - 1 >= 0 and board[i-1][j-1] == 'B':
                        yield f'{i-1}{j-1}{i}{j}'
            if player == 'B':
                if board[i][j] == 'B':
                    if i + 1 < 4 and board[i+1][j] == '.':
                        yield f'{i+1}{j}{i}{j}'
                    if i + 1 < 4 and j - 1 >= 0 and board[i+1][j-1] == 'W':
                        yield f'{i+1}{j-1}{i}{j}'
                    if i + 1 < 4 and j + 1 < 4 and board[i+1][j+1] == 'W':
                        yield f'{i+1}{j+1}{i}{j}'
    return []

def make_move(board, move):
    i1, j1, i2, j2 = int(move[0]), int(move[1]), int(move[2]), int(move[3])
    new_board = copy.deepcopy(board)
    if board[i2][j2] == '.':
        return None
    if move in legal_moves(board, board[i2][j2]):
        new_board[i1][j1] = board[i2][j2]
        new_board[i2][j2] = '.'
        return new_board
    return None
    

board = [
    ['B', 'B', 'B', 'B'],
    ['.', '.', '.', '.'],
    ['.', '.', '.', '.'],
    ['W', 'W', 'W', 'W']
]
board1 = make_move(board, '2030')
print(board1, board)

state = State(board, 'W')
print(state.value)
