(import "../lib/unit.scm" :as Unit)

(Unit.test "byte-vector"
    (Unit.assert-equals (*builtin*.byte-vector-count (*builtin*.byte-vector)) 0)

    (const bv' (*builtin*.byte-vector 1 #\2 3))

    (Unit.assert-equals (*builtin*.byte-vector-count bv') 3)

    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 0) 1)
    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 1) 50)
    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 2) 3)

    (Unit.assert-equals (*builtin*.byte-vector-nth bv' (- 1)) 0)
    (Unit.assert-equals (*builtin*.byte-vector-nth bv' 4) 0)
)

(Unit.test "incorrect argument type signal"
    (const inputs
        (list () #t #f 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) car and)
    )

    (for-each inputs (proc (input)
        (Unit.assert-signal-name (*builtin*.byte-vector 1 2 input 3 4) 'InvalidArgument)
    ))
)
