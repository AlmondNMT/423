import random
def insert(A):
    n = len(A)
    for i in range(1, n):
        key = A[i]
        j = i - 1
        while j > -1 and A[j] > key:
            A[j+1] = A[j]
            j -= 1
        A[j+1] = key
    print(i)

A = [random.randint(-50, 50) for i in range(10)]
print(A)
insert(A)
print(A)
