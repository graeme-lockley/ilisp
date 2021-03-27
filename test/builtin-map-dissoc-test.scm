(import "../lib/unit.scm" :as Unit)

(Unit.test "map-dissoc"
    (Unit.assert-equals (*builtin*.map-dissoc {}) {})
    (Unit.assert-equals (*builtin*.map-dissoc {:bob 1 :mary 2} :fred) {:bob 1 :mary 2})
    (Unit.assert-equals (*builtin*.map-dissoc {:bob 1 :mary 2 :fred 10} :fred) {:bob 1 :mary 2})
    (Unit.assert-equals (*builtin*.map-dissoc {:bob 1 :mary 2 :fred 10} :fred :bob) {:mary 2})
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.map-assoc) 'InvalidArgument)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list () '(1 2 3) #t #f 1 #\Q 'hello "hello" :name [] [1 2 3] (proc (n) (+ n 1)) car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.map-dissoc v) 'InvalidArgument)
        )
    )
)
