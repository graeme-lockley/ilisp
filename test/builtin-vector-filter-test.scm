(import "../lib/unit.scm" :as Unit)

(Unit.test "vector-filter"
    (const (modulo m n) (- m (* n (/ m n))))
    (const (even n) (= (modulo n 2) 0))

    (Unit.assert-equals (*builtin*.vector-filter [] even) [])
    (Unit.assert-equals (*builtin*.vector-filter [1] even) [])
    (Unit.assert-equals (*builtin*.vector-filter [1 2] even) [2])
    (Unit.assert-equals (*builtin*.vector-filter [1 2 3 4 5] even) [2 4])
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.vector-filter) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-filter "Hello") 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-filter "Hello" 2 3) 'ExpectedArgumentCount)
)

(Unit.test "incorrect type signal"
    (Unit.assert-signal-name (*builtin*.vector-filter 'hello car) 'InvalidArgument)
    (Unit.assert-signal-name (*builtin*.vector-filter "hello" and) 'InvalidArgument)
)
