(import "../lib/unit.scm" :as Unit)

(Unit.test "map->list"
    (Unit.assert-equals (*builtin*.map->list {}) ())
    (Unit.assert-equals (*builtin*.map->list {:a 1 :b 2 :c 3}) (list (*builtin*.pair :a 1) (*builtin*.pair :b 2) (*builtin*.pair :c 3)))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.map->list) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.map->list 1 2) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list () '(1 2 3) #t #f 1 #\Q 'hello "hello" :name [] [1 2 3] (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.map->list v) 'InvalidArgument)
        )
    )
)
