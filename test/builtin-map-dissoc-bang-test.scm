(import "../lib/unit.scm" :as Unit)

(Unit.test "map-dissoc!"
    (const v (*builtin*.mutable-map :bob 1 :fred 10 :mary 2))

    (*builtin*.map-dissoc! v :fred)
    (Unit.assert-equals v {:bob 1 :mary 2})

    (*builtin*.map-dissoc! v :jimmy)
    (Unit.assert-equals v {:bob 1 :mary 2})

    (*builtin*.map-dissoc! v :bob :mary)
    (Unit.assert-equals v {})
)

(Unit.test "map-dissoc! signal"
    (Unit.assert-signal-name (*builtin*.map-dissoc! {:bob 1} :bob) 'ValueIsImmutable)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.map-dissoc!) 'InvalidArgument)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list () '(1 2 3) #t #f 1 #\Q 'hello "hello" :name [] [1 2 3] (proc (n) (+ n 1)) car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.map-dissoc! v) 'InvalidArgument)
        )
    )
)
