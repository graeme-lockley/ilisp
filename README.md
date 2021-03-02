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


`iLisp-` supports the following set of data types:

- Nil

- Symbol 
- Keyword

- Boolean
- Character
- Byte
- Integer
- String

- Atom
- Pair
- Vector
- Byte Vector
- Map

- Function

## Influences 

The [mal (Make-a-Lisp)](https://github.com/kanaka/mal) project is a brilliant place to learn Lisp and understand what a Lisp implementation entails.  Huge shout-out to this project.

Paul Graham's [note on a modern dialect of Lisp](http://paulgraham.com/bel.html) is well thought out and represents an unconstrained rethink of what a modern dialect would encompass.

Both [Clojure](https://clojure.org) and [Rocket](https://racket-lang.org), as modern implementations, represent state-of-the art with well established eco-systems.

[Common Lisp](https://en.wikipedia.org/wiki/Common_Lisp) and [Scheme](https://en.wikipedia.org/wiki/Scheme_%28programming_language%29) remind us of Lisp's rich heritage and serve as a deep source of knowledge, understanding and insight.

Finally Harold Abelson, Gerald Jay Sussman and Julie Sussman's masterpiece [*Structure and Interpretation of Computer Programs*](https://en.wikipedia.org/wiki/Structure_and_Interpretation_of_Computer_Programs) which, in my opinion, every computer programmer should read at least once.
