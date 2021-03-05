# iLisp

Over the couple of months I have developed an interest in the philosophy behind Lisp and the thinking behind Lisp.  This is unusual for me as I have been characterised as a statically typed nut however, the rich history and durability of Lisp, tells a different story.  My effort to grok Lisp has resulted in this project - a boostrapped Lisp implementation from the groundup.


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

The following lists `iLisp-` influences with aspects contributed by each influencer..

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
| Symbol | Symbols are objects whose values are equivalent if and only if their names are spelled the same way.  So `'hello` and `'Hello` are different symbols whilst `'world` and `'world` are equivalent. |
| Keyword | Keywords are objects similar to symbols with the added properity that they evaluate to themselves.  So `(((:bob)))` evaluates to `:bob`. Keywords typically have the form colon (`:`) followed by a sequence of identifier characters.  Note that it is not mandatory that a keyword name starts with colon as a keyword can be constructed in the following way `(symbol "hello")` returning a keyword named `hello`.|
| Boolean | Composed of the two keywords `#t` and `#f`. Like Scheme *truthy* is any value except `#f` while *falsy* is only `#f`. |
| Character | A representation of 8 bit ASCII (Extended ASCII). Each value is held as an integer in the range 0..255 with this value being coercible into integers. Constant characters are respresented using Scheme's notation of a `#\` prefix followed by the character.  So checking that a variable `c` is a numeric character would be expressed as `(<=  #\0 c #\9)`.  A literal character can also be expressed as `#x`*\<hex scalar\>* with *\<hex scaler\>* being the character's ASCII.  |
| Integer | A 32 bit signed value. |
| String | A sequence of characters. A literal string is a sequence of character delmitted with with quotation marks (`"`). Within a string literal, various escape sequences represent characters other than themselves: <ul><li>`\t` character tabulation `#\x09`</li><li>`\n`: linefeed `#\0a`</li><li>`\r`: return `\#x0d`</li><li>`\\`: backslash</li><li>`\x`*\<hex scalar\>*`;`: specified character with the ASCII code of *\<hex scalar*\></li></ul>|
| Atom | Atoms provide a way to manage shared, synchronous, independent, mutable state and are the primary mechansim in handling mutation. |
| Pair | A pair is an object with two fields - *car* and *cdr*. Pairs are created using the procedure `cons` with each field accessible using the procedures `car` and `cdr` respectively. Pairs are used primarily to represent lists. A list is defined recursively as either *null* or a pair whose *cdr* is a list. Pairs created using the procedure `cons` are immutable.  A mutable pair can be created using the procedure `mcons` with the `car` and `cdr` fields being mutated using the procedures `set-car!` and `set-cdr!` respectively. Other than supporting mutation, pairs created using `mcons` are simliar to `cons` pairs in every other way. Literal lists are represented by enclosing the elements with parenthesis and separating with whitespace.  The expression `(cons 1 (cons 2 (cons 3 (cons 4 ()))))` can literally be represented as `(1 2 3 4)` |
| Vector | Vectors are heterogeneous structures whose elements are indexed by integers. A vector typically occupies less space than a list of the same length, and the average time needed to access a randomly chosen element is typically less for the vector than for the list. Literal vectors are represented by enclosing the elements with square brackers and separating with whitespace. The literal vector `[1 2 3 4 5]` is a vector with 5 elements indexed from 0 to 4 inclusive.  Vectors are immutable however a mutable vector can be formed by passing a vector to the procedure `->mutable`.  So the following expression `(->mutable [1 2 3 4 5])` will return a mutable vector. |
| Byte Vector | Byte vectors represent blocks of binary data. They are fixed-length sequences of bytes, where a byte is an exact integer in the range from 0 to 255 inclusive. A bytevector is typically more space-efficient than a vector containing the same values. |
| Procedure | A procedure is how things get done in `iLisp-`.  There are two kinds of procedures - native procedures and compound procedures.  A native procedure is provided by the `iLisp-` runtime system whilst a compound procedure is composed using a sequence of expressions and represented using the `proc` form. As an aside it pains me to introduce the `proc` form and define increment as `((proc n) (+ n 1))` rather than using Scheme's `lambda` form `((lambda n) (+ n 1))` or Clojure's `fn` form `((fn n) (+ n 1))`.  I have elected to go with the `proc` form for the following reasons: <ul><li>All the Lisp flavours refer to behavior as a procedure rather than a function or method.</li> <li>Functions are grounded in maths and the association to many is they are a mapping from a domain to a range, side-effect free and are total.  A language based on functions, like Haskell, is rather explicit about this.</li><li>These behaviours are procedures: they are a step-by-step sequence of instructions, they can mutate both internal and external state, they may fail in execution and they make no pretence as to being a function or relation.</li></ul>The difficulty in changing from `lambda`/`fn` to `proc` is sentiment and, on reflection, appropriation of the concept felt unnecessary. |


## References 

The [mal (Make-a-Lisp)](https://github.com/kanaka/mal) project is a brilliant place to learn Lisp and understand what a Lisp implementation entails.  Huge shout-out to this project.

Paul Graham's [note on a modern dialect of Lisp](http://paulgraham.com/bel.html) is well thought out and represents an unconstrained rethink of what a modern dialect would encompass.

Both [Clojure](https://clojure.org) and [Rocket](https://racket-lang.org), as modern implementations, represent state-of-the art with well established eco-systems.

[Common Lisp](https://en.wikipedia.org/wiki/Common_Lisp) and [Scheme](https://en.wikipedia.org/wiki/Scheme_%28programming_language%29) remind us of Lisp's rich heritage and serve as a deep source of knowledge, understanding and insight.

Finally Harold Abelson, Gerald Jay Sussman and Julie Sussman's masterpiece [*Structure and Interpretation of Computer Programs*](https://en.wikipedia.org/wiki/Structure_and_Interpretation_of_Computer_Programs) which, in my opinion, every computer programmer should read at least once.
