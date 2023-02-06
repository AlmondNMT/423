import sympy as sp
sp.interactive.printing.init_printing(use_latex=True)
from sympy.abc import n, x, t

expr = 4 * sp.integrate((x ** 2 - x) * sp.sin(n * sp.pi * x), (x, 0, 1))
