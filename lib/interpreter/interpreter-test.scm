(import "./environment.scm" :as Environment)
(import "./frame.scm" :as Frame)
(import "./interpreter.scm" :as Interpreter)
(import "../unit.scm" :as Unit)

(define (mk-env)
    (Environment.extend 
        Environment.the-empty-environment 
        (list 'a 'x)
        (list "hello" 10)
    )
)

(Unit.test "eval self evaluating"
    (Unit.assert-equals (Interpreter.eval Environment.the-empty-environment 10) 10)
)

(Unit.test "eval variable"
    (define env (mk-env))

    (Unit.assert-equals (Interpreter.eval env 'x) 10)

    (Unit.assert-signal (Interpreter.eval env 'y) (fn (s) (do
        (Unit.assert-equals (car signal) 'UnboundVariable)
        (Unit.assert-equals (get (cdr signal) :name) 'y)
        (Unit.assert-equals (get (cdr signal) :procedure) 'lookup-variable-name)
    )))
)

(Unit.test "eval quoted"
    (Unit.assert-equals (Interpreter.eval Environment.the-empty-environment (list 'quote "hello world")) "hello world")
    (Unit.assert-equals (Interpreter.eval Environment.the-empty-environment (list 'quote "hello" "world")) "hello")
)

(Unit.test "eval assignment"
    (define env (mk-env))
    
    (Unit.assert-equals (Environment.lookup-variable-value env 'a) "hello")
    (Interpreter.eval env (list 'set! 'a "bye"))
    (Unit.assert-equals (Environment.lookup-variable-value env 'a) "bye")

    (Unit.assert-signal (Interpreter.eval env (list 'set! 'b "bye")) (fn (s) (do
        (Unit.assert-equals (car signal) 'UnboundVariable)
        (Unit.assert-equals (get (cdr signal) :name) 'b)
        (Unit.assert-equals (get (cdr signal) :procedure) 'set-variable-value!)
    )))
)

(Unit.test "eval define variable"
    (define env (mk-env))
    
    (Interpreter.eval env (list 'define 'b "bye"))

    (Unit.assert-equals (Environment.lookup-variable-value env 'b) "bye")
)

(Unit.test "eval define procedure"
    (Interpreter.eval env (list 'define (list 'b 'x) 'x))
    (Unit.assert-equals (Environment.lookup-variable-value env (symbol "b")) (list 'procedure (list 'x) (list 'x) env))
)

(Unit.test "eval if"
    (Unit.assert-equals (Interpreter.eval env (list 'if 1 2 3)) 2)
    (Unit.assert-equals (Interpreter.eval env (list 'if () 2 3)) 3)
    (Unit.assert-equals (Interpreter.eval env (list 'if () 2)) ())
)

(Unit.test "eval do"
    (define env (mk-env))
    
    (Unit.assert-equals (Interpreter.eval env (list 'do)) ())
    (Unit.assert-equals (Interpreter.eval env (list 'do (list 'define 'b "bye") 'b)) "bye")

    (Unit.assert-equals (Environment.lookup-variable-value env 'b) "bye")
)
