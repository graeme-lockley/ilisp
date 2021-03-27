(import "../lib/unit.scm" :as Unit)

(Unit.test "character->string"
    (Unit.assert-equals (*builtin*.character->string #\A) "A")
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.character->string) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.character->string 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list #t #f 1 :bob 'hello [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.character->string v) 'InvalidArgument)
        )
    )
)
