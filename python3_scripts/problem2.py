import matplotlib.pyplot as plt
import random
import time
def measure_times(A):
    times = []
    for i in range(1, 10000, 1):
        A = [random.randint(-10000, 10000) for j in range(i)]
        start = time.time()
        XSORT(A, 0, len(A) - 1)
        elapsed = time.time() - start
        print(elapsed)
        times.append(elapsed)
    return times

def XSORT(A, lo, hi):
    if lo < hi:
        XSORT(A, lo, hi - 1)
        i = hi - 1
        key = A[hi]
        while i >= lo and A[i] > key:
            A[i + 1] = A[i]
            i -= 1
        A[i + 1] = key


if __name__ == "__main__":
    A = [12, 10, 22, 5]
    XSORT(A, 0, 3)
