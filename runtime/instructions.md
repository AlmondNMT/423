* bitwise operators:
    * ixor, iand, ior, icomp
    * | -> ior
    * & -> iand
    * ^ -> ixor
    * ishift(1, 2) or ishift(1, -2)


* tables:
    * x := table()
    * x["asdf"] := 1
    * printing tables: 
        * printing keys: every write(key(x))
            * i^th key: 
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

* length of iterables \*L

* variadic procedures
  procedure test(x[])
    every k := !x do {
        write(k)
    }
  end

* notes: 
    * Unicon allows you to overwrite builtin symbols with no warning, just like
      python
