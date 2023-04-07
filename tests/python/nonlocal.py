def f():
    x = 1
    def g():
        nonlocal x
        x = 2
    g()
    return x