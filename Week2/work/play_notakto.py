import pygame
import random
import json
import argparse

def draw_x(x_pos, y_pos, s, offset_x):
    # x_pos += offset_x
    pygame.draw.line(s, X_COLOR, (x_pos + 25, y_pos + 25), (x_pos + 75, y_pos + 75), 10)
    pygame.draw.line(s, X_COLOR, (x_pos + 25, y_pos + 75), (x_pos + 75, y_pos + 25), 10)

def draw_boards(s, offset_x):
    global moves, game_over, arial_font, boards, turn
    s.fill(BG_COLOR)

    for board_num in range(2):
        board_offset_x = board_num * offset_x

        # Draw grid lines for the board
        pygame.draw.line(s, BOARD_COLOR, (100 + board_offset_x, 100), (100 + board_offset_x, 400), 5)
        pygame.draw.line(s, BOARD_COLOR, (200 + board_offset_x, 100), (200 + board_offset_x, 400), 5)
        pygame.draw.line(s, BOARD_COLOR, (300 + board_offset_x, 100), (300 + board_offset_x, 400), 5)
        pygame.draw.line(s, BOARD_COLOR, (400 + board_offset_x, 100), (400 + board_offset_x, 400), 5)

        pygame.draw.line(s, BOARD_COLOR, (100 + board_offset_x, 100), (400 + board_offset_x, 100), 5)
        pygame.draw.line(s, BOARD_COLOR, (100 + board_offset_x, 200), (400 + board_offset_x, 200), 5)
        pygame.draw.line(s, BOARD_COLOR, (100 + board_offset_x, 300), (400 + board_offset_x, 300), 5)
        pygame.draw.line(s, BOARD_COLOR, (100 + board_offset_x, 400), (400 + board_offset_x, 400), 5)

        for move in moves[board_num]:
            print(move[0], move[1], s, board_offset_x)
            draw_x(move[0], move[1], s, board_offset_x)

    if game_over:
        img = arial_font.render('Player ' + ('1' if turn else '2') + ' Wins!', True, BOARD_COLOR)
        s.blit(img, (210, 20))

def check_board_win(board):
    # Check rows, columns, and diagonals for a losing line
    lines = [
        [board[0], board[1], board[2]],
        [board[3], board[4], board[5]],
        [board[6], board[7], board[8]],
        [board[0], board[3], board[6]],
        [board[1], board[4], board[7]],
        [board[2], board[5], board[8]],
        [board[0], board[4], board[8]],
        [board[2], board[4], board[6]]
    ]
    for line in lines:
        if line[0] == line[1] == line[2] == 'x':
            return True
    return False

def check_win(board):
    # Check rows, columns, and diagonals for a losing line
    print(check_board_win(board[0]), check_board_win(board[1]))
    print(board[0], board[1])
    if check_board_win(board[0]) and check_board_win(board[1]):
        return True

def make_move(board_num, move):
    global moves, boards, turn, game_over
    moves[board_num].add(move)
    move1 = move
    if move[0] > 500:
        ind = (move1[0] - 600) // 100 + 3 * ((move1[1] - 100) // 100)
        board_num = 1
    else:
        ind = (move1[0] - 100) // 100 + 3 * ((move1[1] - 100) // 100)
        board_num = 0
    print(ind, move1)
    boards[board_num][ind] = 'x'

    if check_win(boards):
        game_over = True

def in_square(x, y, square, offset_x):
    top_left_corner = board_index_to_coordinates_map[square]
    if top_left_corner[0] + offset_x < x < top_left_corner[0] + offset_x + 100 and top_left_corner[1] < y < top_left_corner[1] + 100:
        return True
    return False

def return_square(x, y, offset_x):
    for i in range(18):
        if in_square(x, y, i, offset_x):
            return i
    return None

def move_action(event, last_click_time, square, surface, board_num, offset_x):
    global click_delay, moves, turn, screen
    move_coordinates = board_index_to_coordinates_map[square]

    if (move_coordinates[0], move_coordinates[1], turn) not in moves[board_num]:
        if event.type == pygame.MOUSEBUTTONDOWN:
            current_time = pygame.time.get_ticks()
            if current_time - last_click_time > click_delay:
                make_move(board_num, (move_coordinates[0], move_coordinates[1], turn))
                turn = not turn
                draw_boards(screen, offset_x)
            last_click_time = current_time
        else:
            draw_boards(screen, offset_x)
            # print(move_coordinates[0], move_coordinates[1])
            # draw_x(move_coordinates[0], move_coordinates[1], surface, offset_x)
    return last_click_time

# Command-line argument parsing
parser = argparse.ArgumentParser()
parser.add_argument('--BotPlayer', type=str, help='Bot player (1 or 2)')
parser.add_argument('--BotStrategyFile', type=str, help='JSON file containing strategy')
arguments = parser.parse_args()

# Pygame setup
pygame.init()
screen = pygame.display.set_mode((1000, 500))
pygame.display.set_caption("Notakto")

# Global variables
clock = pygame.time.Clock()
click_delay = 500  # milliseconds
last_click_time = 0
running = True
turn = True
game_over = False
use_policy = False
offset_x = 500

# Define colors
BOARD_COLOR = "black"
X_COLOR = "red"
BG_COLOR = "white"
arial_font = pygame.font.SysFont('arialunicode', 36)

boards = [['0'] * 9, ['0'] * 9]
moves = [set(), set()]

board_index_to_coordinates_map = {0: (100, 100), 1: (200, 100), 2: (300, 100),
                                  3: (100, 200), 4: (200, 200), 5: (300, 200),
                                  6: (100, 300), 7: (200, 300), 8: (300, 300),
                                  9: (600, 100), 10: (700, 100), 11: (800, 100),
                                  12: (600, 200), 13: (700, 200), 14: (800, 200),
                                  15: (600, 300), 16: (700, 300), 17: (800, 300)}

# Load bot strategy if provided
if arguments.BotPlayer and arguments.BotStrategyFile:
    use_policy = True
    bot_player = int(arguments.BotPlayer)
    policy = json.load(open(arguments.BotStrategyFile, 'r'))

# Game loop
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_y:
                # Reset game
                boards = [['0'] * 9, ['0'] * 9]
                moves = [set(), set()]
                turn = True
                game_over = False
            elif event.key == pygame.K_n:
                running = False

    if game_over:
        draw_boards(screen, offset_x)
        img = arial_font.render('Play again? y/n', True, BOARD_COLOR)
        screen.blit(img, (210, 430))
    else:
        draw_boards(screen, offset_x)  # Ensure both boards are drawn
        if use_policy and ((turn and bot_player == 1) or (not turn and bot_player == 2)):
            board_strs = [''.join([str(x) for x in board]) for board in boards]
            board_str = board_strs[0] + board_strs[1]
            if board_str not in policy:
                print('Error: Policy does not contain history', board_str)
                exit(1)
            available_plays = policy[board_str]
            random_number = random.uniform(0, 1)
            chosen_play = int(available_plays)
            move_coordinates = board_index_to_coordinates_map[chosen_play]
            print((move_coordinates[0], move_coordinates[1], turn) not in moves[board_num])
            last_click_time = move_action(event, last_click_time, chosen_play, screen, board_num, offset_x)
            if (move_coordinates[0], move_coordinates[1], turn) not in moves[board_num]:
                make_move(board_num, (move_coordinates[0], move_coordinates[1], turn))
                turn = not turn
                draw_boards(screen, offset_x)
        else:
            x, y = pygame.mouse.get_pos()
            surface = pygame.Surface((1000, 500))
            surface.set_alpha(100)
            for board_num in range(2):
                board_offset_x = board_num * offset_x
                square = return_square(x, y, board_offset_x)
                # print(square)
                if square is not None:
                    last_click_time = move_action(event, last_click_time, square, surface, board_num, board_offset_x)
        draw_boards(screen, offset_x)

    pygame.display.flip()
    clock.tick(60)

pygame.quit()
