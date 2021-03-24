(import "./environment.scm" :as Environment)
(import "./frame.scm" :as Frame)
(import "./interpreter.scm" :as Interpreter)
(import "../unit.scm" :as Unit)

(const- (mk-env)
    (Environment.extend 
        Environment.the-empty-environment 
        (list 'a 'x 'plus)
        (list "hello" 10 (list 'primitive +))
    )
)

(Unit.test "eval self evaluating"
    (Unit.assert-equals (Interpreter.eval Environment.the-empty-environment 10) 10)
)

(Unit.test "eval variable"
    (const env (mk-env))

    (Unit.assert-equals (Interpreter.eval env 'x) 10)

    (Unit.assert-signal (Interpreter.eval env 'y) (proc (s) (do
        (Unit.assert-equals (car signal) 'UnboundVariable)
        (Unit.assert-equals (map-get (cdr signal) :name) 'y)
        (Unit.assert-equals (map-get (cdr signal) :procedure) 'lookup-variable-name)
    )))
)

(Unit.test "eval quoted"
    (Unit.assert-equals (Interpreter.eval Environment.the-empty-environment (list 'quote "hello world")) "hello world")
    (Unit.assert-equals (Interpreter.eval Environment.the-empty-environment (list 'quote "hello" "world")) "hello")
)

(Unit.test "eval assignment"
    (const env (mk-env))
    
    (Unit.assert-equals (Environment.lookup-variable-value env 'a) "hello")
    (Interpreter.eval env (list 'set! 'a "bye"))
    (Unit.assert-equals (Environment.lookup-variable-value env 'a) "bye")

    (Unit.assert-signal (Interpreter.eval env (list 'set! 'b "bye")) (proc (s) (do
        (Unit.assert-equals (car signal) 'UnboundVariable)
        (Unit.assert-equals (map-get (cdr signal) :name) 'b)
        (Unit.assert-equals (map-get (cdr signal) :procedure) 'set-variable-value!)
    )))
)

(Unit.test "eval define variable"
    (const env (mk-env))
    
    (Interpreter.eval env (list 'define 'b "bye"))

    (Unit.assert-equals (Environment.lookup-variable-value env 'b) "bye")
)

(Unit.test "eval define procedure"
    (const env (mk-env))
    
    (Interpreter.eval env (list 'define (list 'b 'x) 'x))
    (Unit.assert-equals (Environment.lookup-variable-value env (symbol "b")) (list 'procedure (list 'x) (list 'x) env))
)

(Unit.test "eval if"
    (const env (mk-env))
    
    (Unit.assert-equals (Interpreter.eval env (list 'if #t 2 3)) 2)
    (Unit.assert-equals (Interpreter.eval env (list 'if #f 2 3)) 3)
    (Unit.assert-equals (Interpreter.eval env (list 'if #f 2)) ())
)

(Unit.test "eval do"
    (const env (mk-env))
    
    (Unit.assert-equals (Interpreter.eval env (list 'do)) ())
    (Unit.assert-equals (Interpreter.eval env (list 'do (list 'define 'b "bye") 'b)) "bye")

    (Unit.assert-equals (Environment.lookup-variable-value env 'b) "bye")
)

(Unit.test "eval apply primitive"
    (const env (mk-env))
    
    (Unit.assert-equals (Interpreter.eval env (list 'plus 1 2 3)) 6)
)

(Unit.test "eval apply compound"
    (const env (mk-env))
    
    (Interpreter.eval env (list 'define 'inc (list 'lambda (list 'n) (list 'plus 'n 1))))

    (Unit.assert-equals (Interpreter.eval env (list 'inc 1)) 2)
)
