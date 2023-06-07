import test1
test1.test2.a
test1.test2.h.append(1)

def f(a: float, b:int, c: str) -> int:
    x: int
    print("fifa") # needs to throw an error since the function doesn't return an int
    return

a: list = [1, 2, 3]
b = [1, 2, 3][1]
[1, 2, 3].append(1)
(1 + 2 * 3).as_integer_ratio()
c = 2
d = 23
e: str = "asdf"
#a += 1.2   # TypeError
#a += "str" # TypeError

f(1, 2, "asdf") # should throw "'b' not found" error
a[1]
asdf: list = ["asdf", test1, f]
