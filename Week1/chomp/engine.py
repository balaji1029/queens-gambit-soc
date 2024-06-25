import copy

class Bar:
    def __init__(self):
        self.rows = 4
        self.cells = [7, 7, 7, 7]

    def __init__(self, cells):
        self.rows = len(cells)
        self.cells = cells
    
    def __str__(self):
        return '\n'.join(['#'*cell for cell in self.cells])
    
    def __eq__(self, other):
        return self.cells == other.cells
    
    def legal_moves(self):
        moves = []
        for i in range(self.rows):
            for j in range(self.cells[i]):
                if i != 0 or j != 0:
                    moves.append((i, j))
        return moves
    
    def is_terminal(self):
        return sum(self.cells) == 1
    
    def apply_move(self, move):
        new_cells = copy.deepcopy(self.cells)
        for i in range(move[0], self.rows):
            new_cells[i] = min(new_cells[i], move[1])
        return Bar(new_cells)
    
    def get_children(self):
        children = []
        for move in self.legal_moves():
            children.append(self.apply_move(move))
        return children

class State:
    def __init__(self, player, bar):
        self.player = player
        self.bar = bar
        self.value = None
        if self.is_terminal():
            self.value = 1 if self.player == 2 else -1
        else:
            if self.player == 1:
                self.value = max([State(3-self.player, child).value for child in self.bar.get_children()])
            else:
                self.value = min([State(3-self.player, child).value for child in self.bar.get_children()])
    
    def __str__(self):
        return f'{self.player}\'s turn\n{self.bar}'
    
    def __eq__(self, other):
        return self.player == other.player and self.bar == other.bar
    
    def get_children(self):
        children = []
        for child in self.bar.get_children():
            children.append(State(3-self.player, child))
        return children
    
    def is_terminal(self):
        return self.bar.is_terminal()
    
state = State(1, Bar([1,1]))
print(state.value)