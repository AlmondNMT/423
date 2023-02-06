import random

def mergesort(A, p, r):
    if p < r:
        q = (p + r) // 2
        left = mergesort(A, p, q)
        right = mergesort(A, q + 1, r)
        merged = merge(left, right)
        print(f"p: {p+1}\tq: {q+1}\tr: {r+1}\t{merged}")
        return merge(left, right)
    return A[p:r+1]

def merge(left, right):
    m = len(left)
    n = len(right)
    i, j = 0, 0
    B = []
    while i < m and j < n:
        if left[i] <= right[j]:
            B.append(left[i])
            i += 1
        else:
            B.append(right[j])
            j += 1
    for p in range(i, m):
        B.append(left[p])
    for p in range(j, n):
        B.append(right[p])
    return B

A = [12, 10, 22, 5, 8, 14, 15, 6]
print(mergesort(A, 0, len(A)+1))
