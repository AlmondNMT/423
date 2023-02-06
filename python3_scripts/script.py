import matplotlib.pyplot as plt
import numpy as np
import sympy as sp
sp.interactive.printing.init_printing(use_latex=True)
from sympy.abc import a, b, x, y, t, n, rho, phi, N, tau

plt.ion()

def p1():
    u = 3 * sp.exp(-2 * y) * sp.cos(2 * x) + sp.exp(-3 * y) * sp.cos(3 * x)
    return u

def p2():
    def series(x):
        return -2*x**2+x+4-16 / np.pi ** 3 * sum(np.sin((2*n-1) * np.pi * x) / (2*n-1) ** 3 for n in range(1, 100))
    fig, ax = plt.subplots()
    x = np.arange(0, 1, .001)
    l1, = ax.plot(x, 4-x)
    l2, = ax.plot(x, series(x))


def p3():
    A = sp.Function("A")(n)
    u = sp.Sum(A * rho ** (2 * n) * sp.sin(2 * n * phi), (n, 1, N))
    return u


def p4():
    a_n = 3 + 16 * n ** 2
    f = sp.sin(2 * t) * sp.Sum((-1) ** (n + 1) / n, (n, 1, N))
    u = sp.Sum((-1) ** (n + 1) * (a_n * sp.sin(2 * t) - 2 * sp.cos(2 * t) + 2 * sp.exp(-a_n * t)) * sp.sin(2 * n * x) / (n * (a_n ** 2 + 4)), (n, 1, N))
    return f, u

p2()
#f, u = p4()
