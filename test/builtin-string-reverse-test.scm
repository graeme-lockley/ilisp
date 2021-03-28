(import "../lib/unit.scm" :as Unit)

(Unit.test "string-reverse"
    (Unit.assert-equals (*builtin*.string-reverse "") "")
    (Unit.assert-equals (*builtin*.string-reverse "h") "h")
    (Unit.assert-equals (*builtin*.string-reverse "hello") "olleh")
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.string-reverse) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string-reverse "Hello" 2) 'ExpectedArgumentCount)
)

(Unit.test "incorrect type signal"
    (Unit.assert-signal-name (*builtin*.string-reverse 'hello) 'InvalidArgument)
)
