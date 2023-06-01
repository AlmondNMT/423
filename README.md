# Python Compiler
```python
class Consciousness:
    def __init__(self):
        ...
    def __nihil__(self):
        return 
```

## Tasks
- [ ] Type-checking
    - [ ] Imports: transpile imports and link them with main source. 
        - [ ] Create new tree for imports, preserving the old one. 
        - [x] Obtain symbol table for module
        - [ ] Check for circular imports
    - [ ] Function type-checking
        - [x] Verify that function return types match return types of values actually returned
        - [ ] Verify that arguments match the types of formal parameters
        - [ ] Prevent function/classes in expressions without parentheses. This disallows 
              assigning functions to variables, as well as several other complex tasks.
    - [ ] Ensure that assignments are compatible
    - [ ] 
- [ ] Code Generation
    - [ ] Finish building runtime library (in Unicon)
        - [ ] Builtin imports
        - [ ] Builtin classes and functions
    - [ ] Local imports (like import hangman as h. This sort of requires that we can 
          already transpile the imported file into Unicon)
    - [ ] Figure out how to generate code using the tree and symbol tables.
