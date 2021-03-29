(import "../lib/unit.scm" :as Unit)

(Unit.test "vector-reverse"
    (Unit.assert-equals (*builtin*.vector-reverse []) [])
    (Unit.assert-equals (*builtin*.vector-reverse [1]) [1])
    (Unit.assert-equals (*builtin*.vector-reverse [1 2 3]) [3 2 1])
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.vector-reverse) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-reverse 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list #t #f 1 :bob 'hello () '(1 2 3) {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.vector-reverse v) 'InvalidArgument)
        )
    )
)
