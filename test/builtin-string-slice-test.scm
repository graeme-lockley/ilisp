(import "../lib/unit.scm" :as Unit)

(Unit.test "string-slice"
    (Unit.assert-equals (*builtin*.string-slice "hello" 1 1) "")
    (Unit.assert-equals (*builtin*.string-slice "hello" 1 2) "e")
    (Unit.assert-equals (*builtin*.string-slice "hello" 1 4) "ell")
    (Unit.assert-equals (*builtin*.string-slice "hello" 1 10) "ello")
    (Unit.assert-equals (*builtin*.string-slice "hello" 0 3) "hel")
    (Unit.assert-equals (*builtin*.string-slice "hello" (- 10) 3) "hel")

)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.string-slice) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string-slice "hello") 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string-slice "hello" 2) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string-slice "hello" 2 3 4) 'ExpectedArgumentCount)
)

(Unit.test "incorrect type signal"
    (Unit.assert-signal-name (*builtin*.string-slice 'hello 1 1) 'InvalidArgument)
    (Unit.assert-signal-name (*builtin*.string-slice "hello" 'a 1) 'InvalidArgument)
    (Unit.assert-signal-name (*builtin*.string-slice "hello" 1 'b) 'InvalidArgument)
)
