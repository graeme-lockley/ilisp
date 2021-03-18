(import "../lib/unit.scm" :as Unit)

(Unit.test "atom"
    (for-each (list () #t #f 1 #\Q 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) car and) 
        (proc (v) 
            (Unit.assert-equals (*builtin*.atom-dereference (*builtin*.atom v)) v)
        )
    )
)

(Unit.test "arg count signal"
    (const a (*builtin*.atom 1))

    (Unit.assert-signal-name (*builtin*.atom-dereference) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.atom-dereference a 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list () #t #f 1 #\Q 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) car and) 
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.atom-dereference v) 'InvalidArgument)
        )
    )
)
