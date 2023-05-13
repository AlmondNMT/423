def f(x: int) -> float:
    b = 1
    return b

def g() -> float:
    b = 2
    return b

a = 1
a + g()
a = f
