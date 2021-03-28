(import "../lib/unit.scm" :as Unit)

(Unit.test "vector->list"
    (Unit.assert-equals (*builtin*.vector->list []) ())
    (Unit.assert-equals (*builtin*.vector->list [1]) '(1))
    (Unit.assert-equals (*builtin*.vector->list [1 2 3]) '(1 2 3))

    (Unit.assert-falsy (*builtin*.mutable? (*builtin*.vector->list [])))
    (Unit.assert-falsy (*builtin*.mutable? (*builtin*.vector->list [1])))
    (Unit.assert-falsy (*builtin*.mutable? (*builtin*.vector->list [1 2 3])))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.vector->list) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector->list 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list #t #f 1 :bob 'hello () '(1 2 3) {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.vector->list v) 'InvalidArgument)
        )
    )
)
