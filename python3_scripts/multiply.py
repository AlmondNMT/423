def mbd(a: str, d: int):
    a = int(a[::-1])
    return str(a * d)[::-1]

def multiply(a:int, b:int):
    assert type(a) is int
    assert type(b) is int
    a = str(a)[::-1]
    b = str(b)[::-1]
    m = len(b)
    n = len(a)
    assert m < n
    R = mbd(a, int(b[0]))
    for i in range(1, m):
        S = mbd(a, int(b[i]))
        for j in range(0, i):
            S = mbd(S, 2)
            S = mbd(S, 5)
        R = SUM(R, S)
    return int(R[::-1])

def SUM(a: str, b: str):
    return str(int(a[::-1]) + int(b[::-1]))
