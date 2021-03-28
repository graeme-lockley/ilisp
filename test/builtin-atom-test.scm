(import "../lib/unit.scm" :as Unit)

(Unit.test "atom"
    (for-each (list () #t #f 1 #\Q 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and) 
        (proc (v) 
            (Unit.assert-equals (*builtin*.atom-dereference (*builtin*.atom v)) v)
        )
    )
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.atom) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.atom 1 2) 'ExpectedArgumentCount)
)
