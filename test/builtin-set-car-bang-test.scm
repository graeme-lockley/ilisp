(import "../lib/unit.scm" :as Unit)

(Unit.test "set-car!"
    (Unit.assert-equals (*builtin*.set-car! (mcons 1 2) 3) (*builtin*.pair 3 2))
)

(Unit.test "set-car! on immutable pair signal"
    (Unit.assert-signal-name (*builtin*.set-car! (*builtin*.pair 1 2) 3) 'ValueIsImmutable)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.set-car!) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.set-car! (mcons 1 2)) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.set-car! (mcons 1 2) 10 "asd") 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (Unit.assert-signal-name (*builtin*.set-car! 'hello 10) 'InvalidArgument)
)
