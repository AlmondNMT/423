def f():
    x = 1
    def g():
        nonlocal x
        x = 2
    yield g()
    return x