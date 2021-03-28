(import "../lib/unit.scm" :as Unit)

(Unit.test "string-starts-with"
    (Unit.assert-equals (*builtin*.string-ends-with "" "") #t)
    (Unit.assert-equals (*builtin*.string-ends-with "hello" "") #t)
    (Unit.assert-equals (*builtin*.string-ends-with "hello" "lo") #t)
    (Unit.assert-equals (*builtin*.string-ends-with "hello" "hello") #t)

    (Unit.assert-equals (*builtin*.string-ends-with "hello" "Lo") #f)
    (Unit.assert-equals (*builtin*.string-ends-with "hello" "ohello") #f)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.string-ends-with) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string-ends-with "Hello" 1 2) 'ExpectedArgumentCount)
)

(Unit.test "incorrect type signal"
    (Unit.assert-signal-name (*builtin*.string-ends-with 'hello "aa") 'InvalidArgument)
    (Unit.assert-signal-name (*builtin*.string-ends-with "hello" 'aa) 'InvalidArgument)
)
