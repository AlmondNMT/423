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
# a.b = 1 # Disallowed

"""
term: str indicates multiplication of a string with some sequence of integers. 
    Search for the string in the chain, then add the other parts, then gather 
    the rest of the integers and multiply them in the second argument of repl.
"""

"""
In code generation, the trailers have to be handled extremely carefully. XXDD
"""

"""
Imports are nasty. There are two main kinds: builtin imports and user-defined 
imported pyfiles. In the former case, we simply have to compile the runtime libs
and link them with our main source file, ensuring that the names are mangled 
correctly. For example, 

`import random`

should make the following names visible in the Unicon space across all user-def
source files:
    `random__choice`
    `random__randint`
In the actual source for runtime/random.icn, the functions are all named in this
manner. The same is true for `math`. When an imported user-def imports the same 
builtin as our source pyfile, it should be accessible for both. That is, we 
should be able to do the following:
`
import name

a = name.random.randint(-1, 1)
`
This should translate into 
`
a := random__randint(-1, 1)
`

However in another case, we may have three files: test1.py, main.py, which 
imports name.py, which contains a list called `asdf`
name.py looks like this:
`
import test1

def f(x):
    return x ** 2

asdf = [1, 2, 3]
asdf.append(4)
`

and main.py:
`
import name
name.asdf.append(5)
`

and test1.py
`

`

When we reach `import name` during semantic analysis, name.py should then 
be processed, except we won't generate a main procedure. Rather, we will 
mangle the names of all functions inside it, add them to our code list, then 
return a struct containing all of the statements that would have gone into 
a main procedure if we had compiled name.py directly. This struct will be 
appended 
"""

"""
a[1][2][3][4].append(2).append(4) # this will not occur in the 107 test files.
However, a[1][2] might occur. So this is only case to handle
"""
