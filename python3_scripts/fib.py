import sys
n = int(sys.argv[1])

def fib(n):
    if n < 2: return n
    f = [0, 1]
    for i in range(n):
        index = i % 2
        f[index] = f[index] + f[(index + 1) % 2]
    return f[(index + 1) % 2]

def rec(n):
    array = [-1 for i in range(n + 1)]
    return fib_rec(n, array)

def fib_rec(n, array):
    if n < 2:
        return n
    if array[n-1] != -1:
        a_n1 = array[n-1]
    else:
        a_n1 = fib_rec(n-1, array)
        array[n-1] = a_n1
    if array[n-2] != -1:
        a_n2 = array[n-2]
    else:
        a_n2 = fib_rec(n-2, array)
        array[n-2] = a_n2
    return a_n1 + a_n2

print(rec(n))
