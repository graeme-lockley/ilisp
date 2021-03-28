(import "../lib/unit.scm" :as Unit)

(Unit.test "byte-vector-nth"
    (const bv' (*builtin*.byte-vector 1 #\2 3))

    (Unit.assert-equals (*builtin*.byte-vector-count bv') 3)

    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 0) 1)
    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 1) 50)
    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 2) 3)

    (Unit.assert-equals (*builtin*.byte-vector-nth bv' (- 1)) 0)
    (Unit.assert-equals (*builtin*.byte-vector-nth bv' (- 1) ()) ())
    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 4) 0)
    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 4 ()) ())
)

(Unit.test "arg 0 incorrect type signal"
    (const inputs
        (list () #t #f 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and)
    )

    (for-each inputs (proc (input)
        (Unit.assert-signal-name (*builtin*.byte-vector-nth input 0) 'InvalidArgument)
    ))
)

(Unit.test "arg 1 incorrect type signal"
    (const inputs
        (list () #t #f 'hello "hello" #\a :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and)
    )

    (for-each inputs (proc (input)
        (Unit.assert-signal-name (*builtin*.byte-vector-nth input 0) 'InvalidArgument)
    ))
)

(Unit.test "argument count signal"
    (Unit.assert-signal-name (*builtin*.byte-vector-nth) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.byte-vector-nth (*builtin*.byte-vector)) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.byte-vector-nth (*builtin*.byte-vector) 1 0 2) 'ExpectedArgumentCount)
)
