import test1
def f(x: int, y: float) -> str:
    if y > x:
        return 2 * "asdf"
    return x * "fdsa"
print(test1.test2.test3.a.append(8))
print(1 * "asdf")
print(test1.test2.test3.a)
print(test1.test2.test3.a[1])
print(test1.test2.test3.f(1, 2))

for i in range(10):
    if i == 4:
        for j in range(5):
            print(i + j)
        continue
    if i == 9:
        print("fifa")
        break
    print(i)
print(test1.test2.test3.a.append(11))
print(test1.test2.test3.a.remove(11))
print(test1.f(23))
print(test1.test2.f(f(3, 4), f(4, 3)))
print(test1.test2.test3.f(12, 21))
f(3, 4)
a = "fdsadmdld"
print(a.split("fdsa"))
print(a.replace("ds", "wee"))
