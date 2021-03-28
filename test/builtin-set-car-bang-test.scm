(import "../lib/unit.scm" :as Unit)

(Unit.test "pair-car!"
    (Unit.assert-equals (*builtin*.pair-car! (*builtin*.mutable-pair 1 2) 3) (*builtin*.pair 3 2))
)

(Unit.test "pair-car! on immutable pair signal"
    (Unit.assert-signal-name (*builtin*.pair-car! (*builtin*.pair 1 2) 3) 'ValueIsImmutable)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.pair-car!) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.pair-car! (*builtin*.mutable-pair 1 2)) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.pair-car! (*builtin*.mutable-pair 1 2) 10 "asd") 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (Unit.assert-signal-name (*builtin*.pair-car! 'hello 10) 'InvalidArgument)
)
