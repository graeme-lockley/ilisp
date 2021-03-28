(import "../lib/unit.scm" :as Unit)

(Unit.test "set-cdr!"
    (Unit.assert-equals (*builtin*.set-cdr! (mcons 1 2) 3) (*builtin*.pair 1 3))
)

(Unit.test "set-cdr! on immutable pair signal"
    (Unit.assert-signal-name (*builtin*.set-cdr! (*builtin*.pair 1 2) 3) 'ValueIsImmutable)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.set-cdr!) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.set-cdr! (mcons 1 2)) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.set-cdr! (mcons 1 2) 10 "asd") 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (Unit.assert-signal-name (*builtin*.set-cdr! 'hello 10) 'InvalidArgument)
)
