(import "../lib/unit.scm" :as Unit)

(Unit.test "map-assoc!"
    (const v (*builtin*.mutable-map :bob 1 :fred 10 :mary 2))

    (*builtin*.map-assoc! v :fred 100)
    (Unit.assert-equals v {:bob 1 :fred 100 :mary 2})

    (*builtin*.map-assoc! v :jimmy 200)
    (Unit.assert-equals v {:bob 1 :fred 100 :jimmy 200 :mary 2})
)

(Unit.test "map-assoc! signal"
    (Unit.assert-signal-name (*builtin*.map-assoc! {:bob 1} :bob 2) 'ValueIsImmutable)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.map-assoc!) 'InvalidArgument)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list () '(1 2 3) #t #f 1 #\Q 'hello "hello" :name [] [1 2 3] (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.map-assoc! v) 'InvalidArgument)
        )
    )
)
