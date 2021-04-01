(import "../lib/unit.scm" :as Unit)

(Unit.test "byte-vector-nth!"
    (const bv' (*builtin*.byte-vector->mutable-byte-vector (*builtin*.byte-vector 1 #\2 3)))

    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 0) 1)
    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 1) 50)
    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 2) 3)

    (*builtin*.byte-vector-nth! bv' 1 100)

    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 1) 100)
)

(Unit.test "mutability signal"
    (const bv' (*builtin*.byte-vector 1 #\2 3))

    (Unit.assert-signal-name (*builtin*.byte-vector-nth! bv' 1 1) 'ValueIsImmutable)
)

(Unit.test "out of range index signal"
    (const bv' (*builtin*.byte-vector->mutable-byte-vector (*builtin*.byte-vector 1 #\2 3)))

    (Unit.assert-signal-name (*builtin*.byte-vector-nth! bv' (- 1) 100) 'OutOfRange)
    (Unit.assert-signal-name (*builtin*.byte-vector-nth! bv' 3 100) 'OutOfRange)
)

(Unit.test "arg 0 incorrect type signal"
    (const inputs (list () #t #f 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and))

    (for-each inputs (proc (input)
        (Unit.assert-signal-name (*builtin*.byte-vector-nth! input 0 0) 'InvalidArgument)
    ))
)

(Unit.test "arg 1 incorrect type signal"
    (const bv' (*builtin*.byte-vector->mutable-byte-vector (*builtin*.byte-vector 1 #\2 3)))
    (const inputs (list () #t #f 'hello "hello" #\a :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and))

    (for-each inputs (proc (input)
        (Unit.assert-signal-name (*builtin*.byte-vector-nth! bv' input 0) 'InvalidArgument)
    ))
)

(Unit.test "argument count signal"
    (const bv' (*builtin*.byte-vector->mutable-byte-vector (*builtin*.byte-vector 1 #\2 3)))

    (Unit.assert-signal-name (*builtin*.byte-vector-nth!) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.byte-vector-nth! bv') 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.byte-vector-nth! bv' 1) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.byte-vector-nth! bv' 1 0 2) 'ExpectedArgumentCount)
)
