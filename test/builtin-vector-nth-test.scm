(import "../lib/unit.scm" :as Unit)

(Unit.test "vector-nth"
    (Unit.assert-equals (*builtin*.vector-nth [] 1) ())
    (Unit.assert-equals (*builtin*.vector-nth [0] 1) ())
    (Unit.assert-equals (*builtin*.vector-nth [0 1 2 3 4 5] 1) 1)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.vector-nth) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-nth "Hello") 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-nth "Hello" 2 3) 'ExpectedArgumentCount)
)

(Unit.test "incorrect type signal"
    (Unit.assert-signal-name (*builtin*.vector-filter 'hello 1) 'InvalidArgument)
    (Unit.assert-signal-name (*builtin*.vector-filter [1 2 3] and) 'InvalidArgument)
)
