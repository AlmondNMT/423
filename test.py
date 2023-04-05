import os
def f(x) -> int:
    return x
class Obj:
    b: int
    b = 1

class Bass:
    a: Obj
    a = Obj()

c = Bass()
d: Any
#d = c.a.b
