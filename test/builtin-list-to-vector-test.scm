(import "../lib/unit.scm" :as Unit)

(Unit.test "list->vector"
    (Unit.assert-equals (*builtin*.list->vector ()) [])
    (Unit.assert-equals (*builtin*.list->vector (list 1 2 3 4)) [1 2 3 4])

    (Unit.assert-falsy (*builtin*.mutable? (*builtin*.list->vector (list 1 2 3))))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.list->vector) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.list->vector 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list #t #f 1 #\Q 'hello "hello" :name [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.list->vector v) 'InvalidArgument)
        )
    )
)
