class Engine:

    def __init__(self):
        self.length = int(input('Enter the number of stacks of sticks: '))
        self.stacks = list(map(int, input('Enter the number of sticks in each stack: \n').split()))
        if len(self.stacks) != self.length:
            print('Invalid number of stacks')
            exit(1)
        for i in range(self.length):
            if self.stacks[i] < 1:
                print('Invalid number of sticks in stack')
                exit(1)
        
    def run(self):
        while self.round() == 0:
            print()
            continue
        
    def round(self):
        self.print_stacks()
        print('Player 1\'s turn\n')
        self.take_input()
        if self.length == 0:
            print('Player 1 won!')
            return 1
        self.print_stacks()
        print('Player 2\'s turn\n')
        self.take_input()
        if self.length == 0:
            print('Player 2 won!')
            return 2
        return 0
    
    def print_stacks(self):
        print('Current stacks: \n')
        for i in range(self.length):
            print(f'Stack {i+1}: {self.stacks[i]}')
        print()
    
    def take_input(self):
        stack = int(input('Enter the stack number: '))
        while stack < 1 or stack > self.length:
            print('Invalid stack number')
            stack = int(input('Enter the stack number: '))
        sticks = int(input('Enter the number of sticks to take: '))
        while sticks < 1 or sticks > self.stacks[stack-1]:
            print('Invalid number of sticks')
            sticks = int(input('Enter the number of sticks to take: '))
        self.stacks[stack-1] -= sticks
        if self.stacks[stack-1] == 0:
            self.stacks.pop(stack-1)
            self.length -= 1

if __name__ == '__main__':
    engine = Engine()
    print()
    engine.run()