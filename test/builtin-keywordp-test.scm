(import "../lib/unit.scm" :as Unit)

(Unit.test "keyword?"
    (Unit.assert-truthy (*builtin*.keyword? :name))

    (for-each (list () #t #f 1 #\Q 'hello "hello" '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and) 
        (proc (v)
            (Unit.assert-falsy (*builtin*.keyword? v))
        )
    )
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.keyword?) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.keyword? 1 2) 'ExpectedArgumentCount)
)
