(import "../lib/unit.scm" :as Unit)

(Unit.test "map-assoc"
    (Unit.assert-equals (*builtin*.map-assoc {}) {})
    (Unit.assert-equals (*builtin*.map-assoc {:bob 1 :mary 2} :fred 100) {:bob 1 :fred 100 :mary 2})
    (Unit.assert-equals (*builtin*.map-assoc {:fred 10} :fred 100) {:fred 100})
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.map-assoc) 'InvalidArgument)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list () '(1 2 3) #t #f 1 #\Q 'hello "hello" :name [] [1 2 3] (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.map-assoc v) 'InvalidArgument)
        )
    )
)
