import sys

sys.setrecursionlimit(10**6)

n = int(input())
data = list(map(int, input().split()))

dictionary = dict()

def solution(beg, length):
    if length == 1:
        dictionary[(beg, length)] = (data[beg], 0) if (n - length) % 2 == 0 else (0, data[beg])
        return (data[beg], 0) if (n - length) % 2 == 0 else (0, data[beg])
    # Player 1's turn
    if (n - length) % 2 == 0:
        if (beg+1, length-1) in dictionary:
            case_1 = dictionary[(beg+1, length-1)]
        else:
            case_1 = solution(beg+1, length-1)
        if (beg, length-1) in dictionary:
            case_2 = dictionary[(beg, length)]
        else:
            case_2 = solution(beg, length-1)
        if data[beg] + case_1[0] > data[beg+length-1] + case_2[0]:
            dictionary[(beg, length)] = (case_1[0] + data[beg], case_1[1])
            return (case_1[0] + data[beg], case_1[1])
        else:
            dictionary[(beg, length)] = (case_2[0] + data[beg+length-1], case_2[1])
            return (case_2[0] + data[beg+length-1], case_2[1])
    # Player 2's turn
    else:
        if (beg+1, length-1) in dictionary:
            case_1 = dictionary[(beg+1, length-1)]
        else:
            case_1 = solution(beg+1, length-1)
        if (beg, length-1) in dictionary:
            case_2 = dictionary[(beg, length)]
        else:
            case_2 = solution(beg, length-1)
        if data[beg] + case_1[1] > data[beg+length-1] + case_2[1]:
            dictionary[(beg, length)] = (case_1[0], case_1[1] + data[beg])
            return (case_1[0], case_1[1] + data[beg])
        else:
            dictionary[(beg, length)] = (case_2[0], case_2[1] + data[beg+length-1])
            return (case_2[0], case_2[1] + data[beg+length-1])

answer = solution(0, n)
if sum(data) != answer[0] + answer[1]:
    print(False)
# print(solution(0, n))
if answer[0] > answer[1]:
    print("Player 1 wins")
elif answer[0] < answer[1]:
    print("Player 2 wins")
else:
    print("Its a draw")
