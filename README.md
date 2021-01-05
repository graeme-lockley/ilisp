# iLisp

Over the couple of months I have developed an interest in the philosophy behind Lisp and the thinking behind Lisp.  This is unusual for me as I have been characterised as a statically typed nut however, the rich history and durability of Lisp, tells a different story.  My effort to grok Lisp has resulted in this project - a boostrapped Lisp implementation from the groundup.


This project is an implementation of Lisp with the following technical characteristics:

- The bootstrap implementation is done in C.  The reason for using C is the compiler will be LLVM based so using C will allow a high degree of consistency between the interpreted and compiled code.
- All of the C data structures will be accessible and modifyable from iLisp.
- No 3rd party libraries for anything - keep everything super simple reducing cognitive load.

## Super High-Level Approach

- Build an iLisp- interpreter entirely in C
- Add a garbage collector to the iLisp- C implementation
- Write an iLisp- interpreter in iLisp-
- Write a compiler of iLisp- in iLisp- to native code
- Build an interpreter of iLisp in iLisp-
- Write the iLisp compiler in iLisp

Clearly iLisp- needs to be a subset of iLisp and which needs to be defined.

## Description of iLisp-

iLisp- supports the following set of data types:

- Nil

- Symbol 
- Keyword

- Character
- Number
- String 

- Pair
- Vector
- Map

- Function

## Influences 

The [mal (Make-a-Lisp)](https://github.com/kanaka/mal) project is a brilliant place to learn Lisp and understand what a Lisp implementation entails.  Huge shout-out to this project.

Paul Graham's [note on a modern dialect of Lisp](http://paulgraham.com/bel.html) is well thought out and represents an unconstrained rethink of what a modern dialect would encompass.

Both [Clojure](https://clojure.org) and [Rocket](https://racket-lang.org), as modern implementations, represent state-of-the art with well established eco-systems.

[Common Lisp](https://en.wikipedia.org/wiki/Common_Lisp) and [Scheme](https://en.wikipedia.org/wiki/Scheme_%28programming_language%29) remind us of Lisp's rich heritage and serve as a deep source of knowledge, understanding and insight.

Finally Harold Abelson, Gerald Jay Sussman and Julie Sussman's masterpiece [*Structure and Interpretation of Computer Programs*](https://en.wikipedia.org/wiki/Structure_and_Interpretation_of_Computer_Programs) which, in my opinion, every computer programmer should read at least once.
