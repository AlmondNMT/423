"""
`open` is a python3 built-in function that returns an _io.TextIOWrapper object. 
We need to add class methods to this somehow without making _io.TextIOWrapper 
directly accessible. May just make a `file` builtin class with methods.

expr_stmt: In semantic analysis we want to handle expr_stmt fairly generally. 
Essentially, we want to effectively handle expressions like these:
"""
#a.b.c.d = c = d = e(1)[1](1, 2, 3)

"""
After we have populated the symboltable with identifiers and type information,
we then want to type check expressions like these in a second pass:
"""
a = b = c = 1
a & b & c
a | b | c
a ^ b ^ c
a + b + c
a - b - c
a * b * c
a / b / c
a % b % c
a ** b ** c
#[1, 2, 3] & [2, 3]

"""
In our first pass, we should detect meaningless expressions like the following:
"""
'''
a = 2 = b
a + b = 2
a * b = 2
'''
