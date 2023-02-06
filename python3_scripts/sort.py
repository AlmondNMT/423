import random

k = 5
seq = []
lengths = []
for i in range(k):
    n = random.randint(5, 20)
    lengths.append(n)
    seq.append(sorted([random.randint(1, 50) for j in range(n)]))
