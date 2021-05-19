(import "../lib/unit.scm" :as Unit)

(Unit.test "vector-slice"
    (Unit.assert-equals (*builtin*.vector-slice [] 0 10) [])
    (Unit.assert-equals (*builtin*.vector-slice [0 1 2 3 4 5 6 7] 2 5) [2 3 4])
    (Unit.assert-equals (*builtin*.vector-slice [0 1 2 3 4 5 6 7] 6 10) [6 7])
    (Unit.assert-equals (*builtin*.vector-slice [0 1 2 3 4 5 6 7] (- 10) 2) [0 1])
    (Unit.assert-equals (*builtin*.vector-slice [0 1 2 3 4 5 6 7] 5 2) [])
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.vector-slice) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-slice 1) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-slice 1 2) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-slice 1 2 3 4) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list #t #f 1 :bob 'hello () '(1 2 3) {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.vector-slice v 1 2) 'InvalidArgument)
        )
    )

    (for-each (list #t #f :bob 'hello () [] [1 2 3] '(1 2 3) {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.vector-slice [1 2 3] v 2) 'InvalidArgument)
            (Unit.assert-signal-name (*builtin*.vector-slice [1 2 3] 2 v) 'InvalidArgument)
        )
    )
)
