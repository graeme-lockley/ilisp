(import "../lib/unit.scm" :as Unit)

(Unit.test "string-count"
    (Unit.assert-equals (*builtin*.string-count "") 0)
    (Unit.assert-equals (*builtin*.string-count "hello") 5)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.string-count) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string-count "Hello" 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (Unit.assert-signal-name (*builtin*.string-count 'hello) 'InvalidArgument)
)
