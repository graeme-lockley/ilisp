(import "../lib/unit.scm" :as Unit)

(Unit.test "vector-sort!"
    (Unit.assert-equals (*builtin*.vector-sort! (*builtin*.vector->mutable-vector [])) [])
    (Unit.assert-equals (*builtin*.vector-sort! (*builtin*.vector->mutable-vector [2])) [2])
    (Unit.assert-equals (*builtin*.vector-sort! (*builtin*.vector->mutable-vector [2 1 3])) [1 2 3])
    (Unit.assert-equals (*builtin*.vector-sort! (*builtin*.vector->mutable-vector [1 0 9 2 3 8 7 4 5 6])) (*builtin*.vector-range 0 9))
)

(Unit.test "immutable"
    (Unit.assert-signal-name (*builtin*.vector-sort! [1 0 9 2 3 8 7 4 5 6]) 'ValueIsImmutable)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.vector-sort!) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-sort! 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list #t #f 1 :bob 'hello () '(1 2 3) {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.vector-sort! v) 'InvalidArgument)
        )
    )
)
