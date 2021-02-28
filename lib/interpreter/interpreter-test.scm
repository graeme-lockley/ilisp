(import "./environment.scm" :as Environment)
(import "./frame.scm" :as Frame)
(import "./interpreter.scm" :as Interpreter)
(import "../unit.scm" :as Unit)

(Unit.test "eval self evaluating"
    (Unit.assert-equals (Interpreter.eval Environment.the-empty-environment 10) 10)
)

(Unit.test "eval variable"
    (Unit.assert-equals (Interpreter.eval (list (Frame.mk (list (symbol "a")) (list 10))) 'a) 10)
)

(Unit.test "eval quoted"
    (Unit.assert-equals (Interpreter.eval Environment.the-empty-environment (list 'quote "hello world")) "hello world")
    (Unit.assert-equals (Interpreter.eval Environment.the-empty-environment (list 'quote "hello" "world")) "hello")
)

(Unit.test "eval assignment"
    (define env 
        (Environment.extend 
            Environment.the-empty-environment 
            (list (symbol "a")) 
            (list "hello")
        )
    )

    (Unit.assert-equals (Environment.lookup-variable-value env (symbol "a")) "hello")
    (Interpreter.eval env (list 'set! (symbol "a") "bye"))
    (Unit.assert-equals (Environment.lookup-variable-value env (symbol "a")) "bye")
)
