dictionary = dict()

n = int(input())
data = list(map(int, input().split()))

for length in range(1, n+1):
    # print(length)
    for beg in range(n-length+1):
        # print(beg, length)
        if length == 1:
            dictionary[(beg, length)] = (data[beg], 0) if (n - length) % 2 == 0 else (0, data[beg])
            continue
        # Player 1's turn
        if (n - length) % 2 == 0:
            case_1 = dictionary[(beg+1, length-1)]
            case_2 = dictionary[(beg, length-1)]
            if data[beg] + case_1[0] > data[beg+length-1] + case_2[0]:
                dictionary[(beg, length)] = (case_1[0] + data[beg], case_1[1])
            else:
                dictionary[(beg, length)] = (case_2[0] + data[beg+length-1], case_2[1])
        # Player 2's turn
        else:
            case_1 = dictionary[(beg+1, length-1)]
            case_2 = dictionary[(beg, length-1)]
            if data[beg] + case_1[1] > data[beg+length-1] + case_2[1]:
                dictionary[(beg, length)] = (case_1[0], case_1[1] + data[beg])
            else:
                dictionary[(beg, length)] = (case_2[0], case_2[1] + data[beg+length-1])
# print(dictionary)
if dictionary[(0, n)][0] > dictionary[(0, n)][1]:
    print("Player 1 wins")
elif dictionary[(0, n)][0] < dictionary[(0, n)][1]:
    print("Player 2 wins")
else:
    print("Its a draw")
        