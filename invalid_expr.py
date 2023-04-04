"""
In our first pass of the tree, we can get all valid assignments, and detect any
invalid assignments. Valid assignments:
"""
a = b
a = b = c = 2
b = a()
c = b[0]

"""
Invalid assignments:
"""
a = b = 2 = c
a(b) = 2
