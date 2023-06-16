* bitwise operators:
    * ixor, iand, ior, icomp

* tables:
    * x := table()
    * x["asdf"] := 1
    * printing tables: 
        * printing keys: every write(key(x))
        * printing values: every write(!x)

* lists
    * a := list()
    * list concatenation: a |||:= [1]

* strings
    * string multiplication: 2 * "asdf" can be performed with repl("asdf", 2)
    * string sum: "asdf" || "fdsa"

* files
    * Very similar to python api
    * f := open(filename, mode)
        * "r" read
        * "w" write
        * "b" bidirectional reading and writing
        * "a" append
        * "c" create and open for writing
        * "t" open in translated mode ???
        * "u" open in untranslated mode ???
        * "p" open pipe
    * write(f, "asdfsdfsdfsdf")
    * read(f)

* loops
    * generate sequence of integers

* length of iterables
