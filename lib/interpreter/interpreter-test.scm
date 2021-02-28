(import "./environment.scm" :as Environment)
(import "./frame.scm" :as Frame)
(import "./interpreter.scm" :as Interpreter)
(import "../unit.scm" :as Unit)

(Unit.test "eval self evaluating"
    (Unit.assert-equals (Interpreter.eval 10 Environment.the-empty-environment) 10)
)

(Unit.test "eval variable"
    (Unit.assert-equals (Interpreter.eval 'a (list (Frame.mk (list (symbol "a")) (list 10)))) 10)
)

(Unit.test "eval quoted"
    (Unit.assert-equals (Interpreter.eval (list 'quote "hello world") Environment.the-empty-environment) "hello world")
    (Unit.assert-equals (Interpreter.eval (list 'quote "hello" "world") Environment.the-empty-environment) "hello")
)