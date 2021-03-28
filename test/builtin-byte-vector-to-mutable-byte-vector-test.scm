(import "../lib/unit.scm" :as Unit)

(Unit.test "byte-vector->mutable-byte-vector"
    (const mbv1 (*builtin*.byte-vector->mutable-byte-vector (*builtin*.byte-vector)))

    (Unit.assert-equals (*builtin*.byte-vector->list mbv1) ())
    (Unit.assert-truthy (*builtin*.mutable? mbv1))

    (const mbv2 (*builtin*.byte-vector->mutable-byte-vector (*builtin*.byte-vector 1 2 3 4 5)))

    (Unit.assert-equals (*builtin*.byte-vector->list mbv2) (list 1 2 3 4 5))
    (Unit.assert-truthy (*builtin*.mutable? mbv2))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.byte-vector->mutable-byte-vector) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.byte-vector->mutable-byte-vector 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (const inputs
        (list () #t #f 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and)
    )

    (for-each inputs (proc (input)
        (Unit.assert-signal-name (*builtin*.byte-vector->mutable-byte-vector input) 'InvalidArgument)
    ))
)
