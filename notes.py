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

'''
a = 2 = b # we're finna forbid these, since they're not in the 107 folder
a + b = 2
a * b = 2

∙ Disallow function/class names from appearing in expr_stmts without parentheses
  (trailer_rep)
∙ For POWER nonterminals, we need to determine the nature of its derivation, 
  i.e., is it a dotted operator, a list access
∙ Disallow assignments to instance attributes, similar to function calls on the LHS
  of an assignment
∙ Do not support chained assignments, even though they are syntactically valid

Typechecking (Essentially the same as B-minor, but with ANY_TYPE
∙ A value may only be assigned to a value of the same type, unless either is
  ANY_TYPE
∙ A function parameter must only accept a value of the same type or ANY_TYPE
∙ The type of a return statement must match the function return type or ANY_TYPE
∙ All binary operators must have compatible types on the left and right hand 
  sides. The type-checking should be commutative.
 + 
  str + str
  int + int
  int + float
  float + float
 - 
  int - int
  int - float
  float - float
 * 
  int * float
  str * int
  int * int
  float * float
 / 
  int / int
  float / int
  float / float
 //
  int // int
  float // int 
  float // float
 
  int ^ int
  int | int
  int & int

 or
  ANY or ANY
 and 
  ANY and ANY

'''
