import test1
import math as ma
import random
## Empty file

def f(x: str, y: int) -> str:
    print("x and y: ", x, y)
    return x * y * 1

def g(x: int) -> float:
    return x ** x * 2


chinga: list = [1, 2]
chinga.append(12)

print(2 * "asdf" * 2 * (3 + 4))
print((1 + 8 - 9 + 10 + 23) * "str")
print(3 * 2 * "asdf" * 4)
print((1 + 3 + 2) * "fifa" * 2) # repl("fifa", (1 + 3 + 2) * 2)
a:str = "asdf" + "fdsa" + (2 * 3 * 4) * "fifa"
print(a)
b = 2 * 3 * 4
print(b)
print([] + [])
print(+-+--1 + 23 + -23)
print(2 << 3 >> 1 >> 1 << 4)
print(2 & 3 & 3 & 6)

print(2 ^ 3 | 3 ^ 4 & 5)
if 1 > 2 or 3 >= 4 < 1 <= 23 == "asdf" == 43 and 3 + 4 < 23 and 4 + 5 or not 12 < 3:
    print(2)

print({"asdf": 1, "fdsa": 2})

print(1 + True)
if True:
    c: str = "asdf" + " fdsa " + " qwer "
    print(1)
    if False and True:
        effa: int = 12
elif True or False:
    print(2)
elif False:
    print(3)
elif 1 < 2:
    print(4)
else:
    print(5, 6, 7, 8)

while False:
    d: str = "asdf"
    2 * 3 * d
    [1, 2, 3 ,4]

for i in range(1, 10, 3):
    e: list = [1, 2, 3]
    e.append(1)
    print(max(e))
    print(i, i, i, i)
    print(random.randint(-1, 1))

x: int
y: str
z: list = [1, 2, []]
"asdf" * 2
print(ma.gcd(2, 3))
j: any = z[3]
j.append(2)
print(j)
print(z[3])
2 // 4 / 3 * 5
j += [1]
x -= 3
x *= 23
x /= 3
x %= 12
x |= 2
x **= x
print(x)
