(import "../lib/unit.scm" :as Unit)

(Unit.test "vector-nth"
    (Unit.assert-equals (*builtin*.vector-nth! (*builtin*.vector->mutable-vector [1]) 0 2) [2])
    (Unit.assert-equals (*builtin*.vector-nth! (*builtin*.vector->mutable-vector [1 2]) 1 5) [1 5])
    (Unit.assert-equals (*builtin*.vector-nth! (*builtin*.vector->mutable-vector [1 2 () 4 5]) 2 3) [1 2 3 4 5])

    (Unit.assert-truthy (*builtin*.mutable? (*builtin*.vector-nth! (*builtin*.vector->mutable-vector [1 2 () 4 5]) 2 3)))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.vector-nth) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-nth "Hello") 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.vector-nth "Hello" 2 3) 'ExpectedArgumentCount)
)

(Unit.test "incorrect type signal"
    (Unit.assert-signal-name (*builtin*.vector-filter 'hello 1) 'InvalidArgument)
    (Unit.assert-signal-name (*builtin*.vector-filter [1 2 3] and) 'InvalidArgument)
)

(Unit.test "out of range"
    (Unit.assert-signal (*builtin*.vector-nth! (*builtin*.vector->mutable-vector [1]) 2 0) 
        (proc (signal) 
            (Unit.assert-equals (*builtin*.pair-car signal) 'OutOfRange)
            (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :index) 2)
            (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :operand) [1])
        )
    )

    (Unit.assert-signal (*builtin*.vector-nth! (*builtin*.vector->mutable-vector [1]) (- 3) 0) 
        (proc (signal)
            (Unit.assert-equals (*builtin*.pair-car signal) 'OutOfRange)
            (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :index) (- 3))
            (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :operand) [1])
            (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) (*builtin*.symbol "*builtin*.vector-nth!"))
        )
    )
)

(Unit.test "immutable"
    (Unit.assert-signal (*builtin*.vector-nth! [1 2 3] 2 0) 
        (proc (signal)
            (Unit.assert-equals (*builtin*.pair-car signal) 'ValueIsImmutable)
            (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :operand) [1 2 3])
            (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) (*builtin*.symbol "*builtin*.vector-nth!"))
        )
    )
)
