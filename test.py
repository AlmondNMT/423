import test1
test1.test2.a
test1.test2.h.append(1)

def f(a, b, c) -> int:
    x: int
    print("fifa") # needs to throw an error since the function doesn't return an int

a: list = [1, 2, 3]
b = 2
c = 2
d = 23
e: float = 1.2
#a += 1.2   # TypeError
#a += "str" # TypeError

f(a, b, c, d, e, f) # should throw "'b' not found" error
a[test1.test2, 2, 3, 4, "asdf", 6.4]
f()
