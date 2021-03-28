(import "../lib/unit.scm" :as Unit)

(Unit.test "string-starts-with"
    (Unit.assert-equals (*builtin*.string-starts-with "" "") #t)
    (Unit.assert-equals (*builtin*.string-starts-with "hello world" "") #t)
    (Unit.assert-equals (*builtin*.string-starts-with "hello world" "hello") #t)
    (Unit.assert-equals (*builtin*.string-starts-with "hello world" "hello world") #t)

    (Unit.assert-equals (*builtin*.string-starts-with "hello world" "Hello") #f)
    (Unit.assert-equals (*builtin*.string-starts-with "hello world" "hello worlds") #f)
    (Unit.assert-equals (*builtin*.string-starts-with "" "h") #f)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.string-starts-with) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string-starts-with "Hello" 1 2) 'ExpectedArgumentCount)
)

(Unit.test "incorrect type signal"
    (Unit.assert-signal-name (*builtin*.string-starts-with 'hello "aa") 'InvalidArgument)
    (Unit.assert-signal-name (*builtin*.string-starts-with "hello" 'aa) 'InvalidArgument)
)
