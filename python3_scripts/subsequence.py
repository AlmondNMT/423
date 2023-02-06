import random

def max_sub(A):
    counts = [1 for i in range(len(A))]
    subseq = []
    q = 1
    for i in range(len(A)):
        for j in range(i + 1, len(A)):
            if A[j] > A[i]:
                counts[j] = max(counts[i] + 1, counts[j])
                if counts[j] > q:
                    subseq.append(A[i])
                    q = counts[j]
    print(subseq)
    return counts

A = [random.randint(-12, 12) for i in range(7)]
print(A)
counts = max_sub(A)
print(counts)
