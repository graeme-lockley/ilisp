(import "../lib/unit.scm" :as Unit)

(Unit.test "byte-vector->list"
    (Unit.assert-equals (*builtin*.byte-vector->list (*builtin*.byte-vector)) ())

    (const content (list 1 2 3))

    (Unit.assert-equals (*builtin*.byte-vector->list (*builtin*.apply *builtin*.byte-vector content)) content)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.byte-vector->list) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.byte-vector->list 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (const inputs
        (list () #t #f 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and)
    )

    (for-each inputs (proc (input)
        (Unit.assert-signal-name (*builtin*.byte-vector->list input) 'InvalidArgument)
    ))
)
