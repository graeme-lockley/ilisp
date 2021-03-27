(import "../lib/unit.scm" :as Unit)

(Unit.test "keyword->string"
    (Unit.assert-equals (*builtin*.keyword->string :name) ":name")
    (Unit.assert-equals (*builtin*.keyword->string (*builtin*.keyword "name")) "name")
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.keyword->string) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.keyword->string 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list #t #f 1 #\Q 'hello [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.keyword->string v) 'InvalidArgument)
        )
    )
)
