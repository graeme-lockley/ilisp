(import "../lib/unit.scm" :as Unit)

(Unit.test "string-filter"
    (Unit.assert-equals (*builtin*.string-filter "hello to the world" (proc (c) (not (< c 32)))) "hello to the world")
    (Unit.assert-equals (*builtin*.string-filter "hello to the world" (proc (c) (not (= c 32)))) "hellototheworld")
    (Unit.assert-equals (*builtin*.string-filter "hello to the world" (proc (c) (> c 110))) "ototwor")
    (Unit.assert-equals (*builtin*.string-filter "hello to the world" (proc (c) (> c 255))) "")
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.string-filter) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string-filter "Hello") 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string-filter "Hello" 2 3) 'ExpectedArgumentCount)
)

(Unit.test "incorrect type signal"
    (Unit.assert-signal-name (*builtin*.string-filter 'hello car) 'InvalidArgument)
    (Unit.assert-signal-name (*builtin*.string-filter "hello" and) 'InvalidArgument)
)
