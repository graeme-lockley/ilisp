(import "../lib/unit.scm" :as Unit)

(Unit.test "pair-cdr!"
    (Unit.assert-equals (*builtin*.pair-cdr! (*builtin*.mutable-pair 1 2) 3) (*builtin*.pair 1 3))
)

(Unit.test "pair-cdr! on immutable pair signal"
    (Unit.assert-signal-name (*builtin*.pair-cdr! (*builtin*.pair 1 2) 3) 'ValueIsImmutable)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.pair-cdr!) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.pair-cdr! (*builtin*.mutable-pair 1 2)) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.pair-cdr! (*builtin*.mutable-pair 1 2) 10 "asd") 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (Unit.assert-signal-name (*builtin*.pair-cdr! 'hello 10) 'InvalidArgument)
)
