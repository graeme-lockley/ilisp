(import "../lib/unit.scm" :as Unit)

(Unit.test "vector-range"
    (Unit.assert-equals (*builtin*.vector-range 1 1) [1])
    (Unit.assert-equals (*builtin*.vector-range 1 0) [1 0])
    (Unit.assert-equals (*builtin*.vector-range 5 10) [5 6 7 8 9 10])
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.vector-range) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-range 1) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-range 1 2 3) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list #t #f :bob 'hello () '(1 2 3) {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.vector-range 0 v) 'InvalidArgument)
            (Unit.assert-signal-name (*builtin*.vector-range v 1) 'InvalidArgument)
        )
    )
)
