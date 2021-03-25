# iLisp

Over the couple of months I have developed an interest in the philosophy behind Lisp and the thinking behind Lisp.  This is unusual for me as I have been characterised as a statically typed nut however, the rich history and durability of Lisp, tells a different story.  My effort to grok Lisp has resulted in this project - a boostrapped Lisp implementation from the ground up.


This project is an implementation of Lisp with the following technical characteristics:

- The bootstrap implementation is done in C.  The reason for using C is the compiler will be LLVM based so using C will allow a high degree of consistency between the interpreted and compiled code.
- All of the C data structures will be accessible and modifyable from iLisp.
- No 3rd party libraries for anything - keep everything super simple reducing cognitive load.

## Super High-Level Approach

In order to bootstrap this project it is necessary to consider a watered down version of iLisp that is rich enough to build an interpreter and compiler for iLisp.  The watered down version will be called `iLisp-`.

The high-level approach then is as follows:

- Build an `iLisp-` interpreter entirely in C
- Add a garbage collector to the `iLisp-` C implementation
- Write an `iLisp-` interpreter in `iLisp-`
- Write a compiler of `iLisp-` in `iLisp-` to native code
- Build an interpreter of iLisp in `iLisp-`
- Write the iLisp compiler in iLisp

Clearly `iLisp-` needs to be a subset of iLisp and which needs to be defined.

## Description of `iLisp-`

The following lists `iLisp-` influences with aspects contributed by each influencer.

| Language/Runtime | Contribution |
|-|-|
| Scheme | <ul><li>Data types</li><li>The rich collection of built-in procedures</li><li>Boolean values rather than just truthy and falsy</li><li>Syntactic flavour</li></ul> |
| JavaScript | <ul><li>A module system that is simple and easy to understand</li><li>The obvious separation between constant (const) and variable (let) declarations.  This is really helpful is it makes intention explicit.</li></ul> |
| Deno | <ul><li>Runtime environment</li></ul> |
| MAL | <ul><li>The basic Lisp interpreter</li></ul> |
| Bel | <ul><li>Thinking about a Lisp flavour in an original way</li></ul> |
| Common Lisp | <ul><li>Source of many ideas</li></ul> |


### Data Types

`iLisp-` supports the following set of data types:

| Data Type | Commentary |
|-|-|
| Null | Literal value of `()` |
| Symbol | Symbols are objects whose values are equivalent if and only if their names are spelt the same way.  So `'hello` and `'Hello` are different symbols whilst `'world` and `'world` are equivalent. |
| Keyword | Keywords are objects similar to symbols with the added property that they evaluate to themselves.  So `(((:bob)))` evaluates to `:bob`. Keywords typically have the form colon (`:`) followed by a sequence of identifier characters.  Note that it is not mandatory that a keyword name starts with colon as a keyword can be constructed in the following way `(symbol "hello")` returning a keyword named `hello`.|
| Boolean | Composed of the two keywords `#t` and `#f`. Like Scheme *truthy* is any value except `#f` while *falsy* is only `#f`. |
| Character | A representation of 8 bit ASCII (Extended ASCII). Each value is held as an integer in the range 0..255 with this value being coercible into integers. Constant characters are respresented using Scheme's notation of a `#\` prefix followed by the character.  So checking that a variable `c` is a numeric character would be expressed as `(<=  #\0 c #\9)`.  A literal character can also be expressed as `#x`*\<hex scalar\>* with *\<hex scaler\>* being the character's ASCII.  |
| Integer | A 32 bit signed value. |
| String | A sequence of characters. A literal string is a sequence of character delmitted with with quotation marks (`"`). Within a string literal, various escape sequences represent characters other than themselves: <ul><li>`\t`: character tabulation `#x09`</li><li>`\n`: linefeed `#x0a`</li><li>`\r`: return `#x0d`</li><li>`\\`: single backslash</li><li>`\"`: double quote `#x22`</li><li>`\x`*\<hex scalar\>*`;`: specified character with the ASCII code of *\<hex scalar*\></li></ul>|
| Atom | Atoms provide a way to manage shared, synchronous, independent, mutable state and are the primary mechansim in handling mutation. |
| Pair | A pair is an object with two fields - *car* and *cdr*. Pairs are created using the procedure `cons` with each field accessible using the procedures `car` and `cdr` respectively. Pairs are used primarily to represent lists. A list is defined recursively as either *null* or a pair whose *cdr* is a list. Pairs created using the procedure `cons` are immutable.  A mutable pair can be created using the procedure `mcons` with the *car* and *cdr* fields being mutated using the procedures `set-car!` and `set-cdr!` respectively. Other than supporting mutation, pairs created using `mcons` are simliar to `cons` pairs in every other way. Literal lists are represented by enclosing the elements with parenthesis and separating with whitespace.  The expression `(cons 1 (cons 2 (cons 3 (cons 4 ()))))` can literally be represented as `(1 2 3 4)` |
| Vector | Vectors are heterogeneous structures whose elements are indexed by integers. A vector typically occupies less space than a list of the same length, and the average time needed to access a randomly chosen element is typically less for the vector than for the list. Literal vectors are represented by enclosing the elements with square brackers and separating with whitespace. The literal vector `[1 2 3 4 5]` is a vector with 5 elements indexed from 0 to 4 inclusive.  Vectors are immutable however a mutable vector can be formed by passing a vector to the procedure `vector->mutable`.  So the following expression `(vector->mutable [1 2 3 4 5])` will return a mutable vector. |
| Byte Vector | Byte vectors represent blocks of binary data. They are fixed-length sequences of bytes, where a byte is an exact integer in the range 0 to 255 inclusive. A byte vector is typically more space-efficient than a vector containing the same values. |
| Procedure | A procedure is how things get done in `iLisp-`.  There are two kinds of procedures - native procedures and compound procedures.  A native procedure is provided by the `iLisp-` runtime system whilst a compound procedure is composed using a sequence of expressions and represented using the `proc` form. As an aside it pains me to introduce the `proc` form and define increment as `((proc n) (+ n 1))` rather than using Scheme's `lambda` form `((lambda n) (+ n 1))` or Clojure's `fn` form `((fn n) (+ n 1))`.  I have elected to go with the `proc` form for the following reasons: <ul><li>All the Lisp flavours refer to behavior as a procedure rather than a function or method.</li> <li>Functions are grounded in maths and the association to many is they are a mapping from a domain to a range, side-effect free and are total.  A language based on functions, like Haskell, is rather explicit about this.</li><li>These behaviours are procedures: they are a step-by-step sequence of instructions, they can mutate both internal and external state, they may fail in execution and they make no pretence as to being a function or relation.</li></ul>The difficulty in changing from `lambda`/`fn` to `proc` is sentiment and, on reflection, appropriation of the concept felt unnecessary. |

### Modules

The `iLisp-` module system has the follow characteristics:

- `iLisp-` code is stored in a file and the file denotes a module.
- Internally a module is composed of a number of declarations.  A declaration can be a value declaration or a macro declaration.
- A declaration binding can be denoted public in which case it can be referenced from a second module.  A declaration that is not public is deemed private and is not referencable from a second module.
- Referencing a module is done by using a file name.  The file name can be absolute, relative to the calling file or a URL.
- Module dependencies may not form a cycle.
- Within a module bindings are formed simultaneously however calculated in sequence.  This implies that values can not be recursively defined however procedures can be mutually recursive.
- Modules are imported using the `import` macro and declarations references using dot notation.

The following examples illustrate the ordering within an `iLisp-` module.

```scheme
(const stuff (+ other-stuff 1))
(const other-stuff 10)
```

Although the declaration above is a little trite and can be resolved by reordering, a real world example would be mututally recursive values constructed for a combinator parser.  The following shows two declarations where the bindings to `odd` and `even` are made when the module is loaded however the value for odd and even is calculated without dereferencing either of these values.

```scheme
(const (even n) 
    (if (= n 0) #t
        (odd (- n 1))
    )
)

(const (odd n)
    (if (= n 0) #f
        (even (- n 1))
    )
)
```

### Standard Forms

The following table desscribes the standard available forms.

| Purpose | Forms |
|-|-|
| Binding declaration | `const`, `const-`, `let`, `let-`, `macro`, `macro-` |
| Conditional evaluation | `if` |
| Iteration | `proc` |
| Sequence | `do` |
| Quoting | `quote`, `unquote`, `quasiquote`, `unquote-splicing` |
| Signals | `try`, `signal` |
| Module | `import` |

### Special Bindings

`iLisp-` includes some special bindings that are the responsibility of the runtime system to make available to all `iLisp-` programs. 

| Name | Purpose |
|-|-|
| `*args*` | A list of the command line arguments passed into the program off of the command line. |
| `*builtin*` | A mapping of the builtin procedures that have been made available through the runtime system to an `iLisp-` program.  The mapping is from string onto native procedure. |
| `*env*` | A mapping of the enclosing shell environment from name onto value.  Both name and value are strings. |
| `*imports*` | A reference to all the imports that have been used within a running program.  In an interpreted environment this declartion would be built as modules are imported and consequently would be variable whereas, in a compiled environment, it would be statically populated by the runtime system before control is handed to procedure execution. |
| `*source-name*` | The absolute file of the module in which this declaration is referenced. A REPL would populate this variable to `**repl**` whilst `eval` can set this declaration to a sensible value. |

### Idioms

To reduce cognitive dissonance when working through a code base the following idioms are encouraged.

| Idiom | Examples |
|-|-|
| All special variables are enclosed with `*` character. | `*args*`, `*builtin*` |
| All predicates have a `?` suffix.  A procedure name containin an `-is-` is considered a bad small and should probably be renamed by removing the `-is-` and adding a `?` suffix. | `empty?`, `string?`, `prime?` |
| Conversions between types or structures have `->` embedded into the name.  | `vector->list`, `->string` | 
| Collections are in the first parameter. | `(map '(1 2 3 4) (proc (n) (+ n 1)))`|
| Constructors that wish to add a prefix verb to communicate intent should use `mk-` | `mk-name` |
| Procedure names are lower case, unless an acronym, and use `-` as a separator | `lookup-variable-value` |
| Module names are title case | `Unit`, `Sequence` |
| Procedures that incure a state side effect have a `!` suffix | `set-variable-value!`, `set-car!` |

### Builtin Procedures

Each `iLisp-` environment is installed with a collection of bindings.  Other than the *Standard Forms*, *Special Bindings* and the builtin procedures installed in `*builtin*`, a prelude file is installed which contains a bindings imported into each module.  The following table lists the procedures available in `*builtin*`.  These procedures are used to the bootstrap each of the libraries and are typically included into prelude other directly or part of a prelude procedure.

Purpose | Procedures
-|-
Construction | `atom`, `byte-vector`, `cons`, `keyword`, `mk-map`, `mcons`, `mutable-byte-vector`, `mutable-map`, `mutable-vector`, `symbol`
Conversion | `char->integer`, `integer->char`, `list->string`, `keyword->string`, `map->list`, `string->mutable-vector`, `list->vector`, `list->mutable-vector`, `vector->list`
Identifiy predicates | `atom?`, `boolean?`, `cond?`, `integer?`, `keyword?`, `list?`, `macro?`, `map?`, `mutable?`, `null?`, `number?`, `proc?`, `symbol?`, `vector?`
Relational operators | `=`, `!=`, `<`, `>`, `<=`, `>=`
Number | `+`, `-`, `*`, `/`
String | `string-count`, `string-filter`, `string-nth`, `string-reverse`, `string-slice`
Pairs and lists | `car`, `cdr`, `list-take`, `set-car!`, `set-cdr!`
Map | `assoc`, `assoc!`, `dissoc`, `dissoc?`, `get`
Vector | `mutable-vector`, `vector`, `vector-count`, `vector-filter`, `vector-nth`, `vector-nth!`, `vector-sort!`
Byte Vector | `byte-vector`, `byte-vector-count`, `byte-vector-nth`, `byte-vector-nth!`, `mutable-byte-vector`
Atom | `atom`, `swap!`, `atom-dereference`, `@`
Functional | `apply`, `eval`
Input/Output | `print`, `println`, `slurp`


## References 

The [mal (Make-a-Lisp)](https://github.com/kanaka/mal) project is a brilliant place to learn Lisp and understand what a Lisp implementation entails.  Huge shout-out to this project.

Paul Graham's [note on a modern dialect of Lisp](http://paulgraham.com/bel.html) is well thought out and represents an unconstrained rethink of what a modern dialect would encompass.

Both [Clojure](https://clojure.org) and [Rocket](https://racket-lang.org), as modern implementations, represent state-of-the art with well established eco-systems.

[Common Lisp](https://en.wikipedia.org/wiki/Common_Lisp) and [Scheme](https://en.wikipedia.org/wiki/Scheme_%28programming_language%29) remind us of Lisp's rich heritage and serve as a deep source of knowledge, understanding and insight.

Finally Harold Abelson, Gerald Jay Sussman and Julie Sussman's masterpiece [*Structure and Interpretation of Computer Programs*](https://en.wikipedia.org/wiki/Structure_and_Interpretation_of_Computer_Programs) which, in my opinion, every computer programmer should read at least once.
