(import "../lib/unit.scm" :as Unit)

(Unit.test "list-count"
    (Unit.assert-equals (*builtin*.list-count ()) 0)
)

(Unit.test "pair-car! on immutable pair signal"
    (Unit.assert-signal-name (*builtin*.pair-car! (*builtin*.pair 1 2) 3) 'ValueIsImmutable)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.list-count) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.list-count 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list #t #f 1 #\Q 'hello "hello" :name [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.list-count v) 'InvalidArgument)
        )
    )
)
