class Hooch:
    r = 1
    def g(self, y):
        return y
class Gooch:
    h = Hooch()
    b = 1
    def f(self, x) -> int:
        return x

class Booch:
    a = Gooch()

c = Booch()
j = 1
c.a.h.g.self
