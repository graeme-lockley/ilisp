(import "../lib/unit.scm" :as Unit)

(Unit.test "integer->character"
    (Unit.assert-equals (*builtin*.integer->character 33) #\!)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.integer->character) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.integer->character 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (const inputs
        (list () #t #f 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and)
    )

    (for-each inputs (proc (input)
        (Unit.assert-signal-name (*builtin*.integer->character input) 'InvalidArgument)
    ))
)
