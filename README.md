# Python Compiler
```python
class Consciousness:
    def __init__(self):
        ...
    def __nihil__(self):
        return 
```

## Tasks

### utils.c
- [x] Fix string deescaping and add tests

### tree.c
- [x] Finish writing alctoken (I think this is done)
- [x] alctree
- [x] Fix memory leak between append\_child and alctoken (IGNORED)
- [ ] 

### GRAMMAR
- [x] Add type hints to grammar

### symtab.c
- [x] Hash function obtained from https://stackoverflow.com/questions/7666509/hash-function-for-string
- [x] Grab for loop variables
- [ ] Verify assignment types
- [ ] Verify the LHS and RHS types for decl\_stmt with an optional assignment
- [x] We are using lists with objects of type any, and loops with an iterator of type any
- [ ] Solve the dotted name problem
- [ ] Imports (we should do this part last)
### symtab.h
