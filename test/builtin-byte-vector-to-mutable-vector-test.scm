(import "../lib/unit.scm" :as Unit)

(Unit.test "byte-vector->mutable-vector"
    (const mbv1 (*builtin*.byte-vector->mutable-vector (*builtin*.byte-vector)))

    (Unit.assert-equals mbv1 [])
    (Unit.assert-truthy (*builtin*.mutable? mbv1))

    (const mbv2 (*builtin*.byte-vector->mutable-vector (*builtin*.byte-vector 1 2 3 4 5)))

    (Unit.assert-equals mbv2 [1 2 3 4 5])
    (Unit.assert-truthy (*builtin*.mutable? mbv2))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.byte-vector->mutable-vector) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.byte-vector->mutable-vector 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (const inputs
        (list () #t #f 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and)
    )

    (for-each inputs (proc (input)
        (Unit.assert-signal-name (*builtin*.byte-vector->mutable-vector input) 'InvalidArgument)
    ))
)
