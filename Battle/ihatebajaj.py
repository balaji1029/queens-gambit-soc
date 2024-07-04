import math
import logging
import chess
import numpy as np
logging.basicConfig(format='%(levelname)s - %(asctime)s - %(message)s', datefmt='%d-%b-%y %H:%M:%S',
                    level=logging.INFO)


def sgn(x):
    if x > 0:
        return 1
    elif x < 0:
        return -1
    else:
        return 0
    

def current_player(board):
    
    if board.turn==chess.WHITE:
        return 1
    else:
        return 2



def is_win(board):

    currentplayer=current_player(board)
    if board.is_checkmate():
        return 3-currentplayer
    return False


def is_draw(board):
    if board.is_stalemate():
        return True
    if board.is_insufficient_material():
        return True
    return False
    

def get_valid_actions(board):
    
    good_moves = []
    capture_moves = []
    other_moves = []
    
    
    
    for move in board.legal_moves:
        sanmove = board.san(move)
        if sanmove[-1]=='#':
            return [sanmove]
        if sanmove[-1] in {'+', 'Q'}:
            good_moves.append(sanmove)
        elif sanmove[1] == 'x':
            capture_moves.append(sanmove)
        else:
            other_moves.append(sanmove)
    
    # Combine all the categorized moves
    valid_actions = good_moves + capture_moves + other_moves
    return valid_actions


def is_terminal_history(board):
    
    if is_win(board) | is_draw(board) :
        return True
    # Feel free to implement this in anyway if needed
    return False
    pass

def get_utility_given_terminal_history(board):
    # Feel free to implement this in anyway if needed
    wincheck=is_win(board)
    
    if wincheck:
        return 10**5 * (3-2*wincheck)
    
    return 0
    pass

def update_history(board, action):
    # In case you need to create a deepcopy and update the history obj to get the next history object.
    
    new_board=board.copy()
    new_board.push_san(action)
    return new_board
    # Feel free to implement this in anyway if needed
    pass


def get_board_str(board):
    return str(board)

def StaticValue(board):

    value=0
    piecediff=PieceDifference(board)/5
    value=value+(sgn(piecediff))*(2*abs(piecediff))
    return value

def PieceDifference(board):
    
    PIECE_VALUES = {
    chess.PAWN: 1,
    chess.KNIGHT: 3,
    chess.BISHOP: 3.2,
    chess.ROOK: 5,
    chess.QUEEN: 9
    }
    SQUARE_PIECE_VALUES = {
    chess.PAWN: 
    [0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 20, 20, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0]
,
    chess.KNIGHT: [
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20,   0,   0,   0,   0, -20, -40,
    -30,   0,  10,  15,  15,  10,   0, -30,
    -30,   5,  15,  20,  20,  15,   5, -30,
    -30,   0,  15,  20,  20,  15,   0, -30,
    -30,   5,  10,  15,  15,  10,   5, -30,
    -40, -20,   0,   5,   5,   0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
]
,
    chess.BISHOP: [
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -10,  10,  10,  10,  10,  10,  10, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,   5,   5,  10,  10,   5,   5, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
]
,
    chess.ROOK: [
     0,   0,   0,   5,   5,   0,   0,   0,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
     5,  10,  10,  10,  10,  10,  10,   5,
     0,   0,   0,   0,   0,   0,   0,   0
]
,
    chess.QUEEN: [
    -20, -10, -10,  -5,  -5, -10, -10, -20,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,   5,   5,   5,   0, -10,
     -5,   0,   5,   5,   5,   5,   0,  -5,
      0,   0,   5,   5,   5,   5,   0,  -5,
    -10,   5,   5,   5,   5,   5,   0, -10,
    -10,   0,   5,   0,   0,   0,   0, -10,
    -20, -10, -10,  -5,  -5, -10, -10, -20
]
    }
    
    white_score = 0
    black_score = 0

    # Iterate through all squares on the board
    for square in chess.SQUARES:
        
        piece = board.piece_at(square)
        if piece:
            valuelist = SQUARE_PIECE_VALUES.get(piece.piece_type, False)
            piecevalue = PIECE_VALUES.get(piece.piece_type,0)
            
            if valuelist:
                if piece.color == chess.WHITE:
                    value=piecevalue*valuelist[square]/100
                    white_score += value
                else:
                    value=piecevalue*valuelist[63-square]/100
                    black_score += value


    ans=round(white_score-black_score,5)
    return ans
    
        
        
    
    
    
    




def alpha_beta_pruning(history_obj, alpha, beta, depth, max_player_flag):
    
    # boardstr=history_obj.fen().split()[0] 
    
    if is_terminal_history(history_obj):
        return get_utility_given_terminal_history(history_obj), None
    if depth==0:
        return StaticValue(history_obj), None 
    
    # if boardstr in board_positions_val_dict:
    #     return board_positions_val_dict[boardstr]
        
    bestmove=""   
    
    if max_player_flag:
        maxEval = -math.inf
        for action in get_valid_actions(history_obj):    
            child=update_history(history_obj,action)
            
            
            # childstr=child.fen().split()[0] 
            
            # if childstr in board_positions_val_dict:
            #     eval,abcd = board_positions_val_dict[childstr]  
            # else:
            #     if ((str(action)[1] == 'x')):
            #         eval,abcd = alpha_beta_pruning(child,alpha,beta,depth-1,False,board_positions_val_dict)
            #     else:
            #         eval,abcd = alpha_beta_pruning(child,alpha,beta,depth-1,False,board_positions_val_dict)
            
            eval,abcd = alpha_beta_pruning(child,alpha,beta,depth-1,False)
                       
            if eval > maxEval:
                bestmove=str(action) 
                           
            maxEval = max(maxEval, eval)
            alpha = max(alpha,eval)
            
            if beta <= alpha:
                break
            
        # board_positions_val_dict[boardstr]=maxEval,bestmove
        return maxEval,bestmove
    
    else:
        minEval = math.inf
        for action in get_valid_actions(history_obj):
            child=update_history(history_obj,action)
            
            # childstr=child.fen().split()[0] 
            
            # if childstr in board_positions_val_dict:
            #     eval,abcd = board_positions_val_dict[childstr]
            # else:    
            #     if ((str(action)[1] == 'x')) :
            #         eval,abcd = alpha_beta_pruning(child,alpha,beta,depth-1,True,board_positions_val_dict)
            #     else:
            #         eval,abcd = alpha_beta_pruning(child,alpha,beta,depth-1,True,board_positions_val_dict)
            
            eval,abcd = alpha_beta_pruning(child,alpha,beta,depth-1,True)

            if eval < minEval:
                bestmove=str(action)
            
            minEval = min(minEval, eval)
            
            beta = min(beta,eval)
            if beta <= alpha:
                break
        # board_positions_val_dict[boardstr]=minEval,bestmove
        return minEval,bestmove
        
    
    return -2