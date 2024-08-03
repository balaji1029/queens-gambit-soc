import json

with open('mate_in_4.json') as f:
    data = json.load(f)

with open('m8n4.txt', 'w') as f:
    for puzzle in data:
        f.write(puzzle + '\n')