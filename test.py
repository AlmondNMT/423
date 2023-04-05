def f(x) -> int:
    return x
class Obj:
    b: int
    b = 1

class Bass:
    a: Obj
    a = Obj()

c = Bass()
d: int
c.a.b
