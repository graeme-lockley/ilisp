(import "../lib/unit.scm" :as Unit)

(Unit.test "atom ="
    (const atom-0 (*builtin*.atom 0))
    (const atom-1 (*builtin*.atom "Hello World"))
    (const atom-2 (*builtin*.atom (list :Bob :Mary)))

    (Unit.assert-truthy (= atom-0 (*builtin*.atom 0)))
    (Unit.assert-falsy (= atom-0 (*builtin*.atom 1)))

    (Unit.assert-truthy (= atom-1 (*builtin*.atom "Hello World")))
    (Unit.assert-falsy (= atom-1 (*builtin*.atom "Bye Bye Love")))

    (Unit.assert-truthy (= atom-2 (*builtin*.atom (list :Bob :Mary))))
    (Unit.assert-falsy (= atom-2 (*builtin*.atom ())))  
    (Unit.assert-falsy (= atom-2 (*builtin*.atom (list :Mary :Luke))))
)

(Unit.test "byte-vector ="
    (const bv-0 (*builtin*.byte-vector))
    (const bv-1 (*builtin*.byte-vector 1))
    (const bv-3 (*builtin*.byte-vector 1 2 3))

    (Unit.assert-truthy (= bv-0 (*builtin*.byte-vector)))
    (Unit.assert-truthy (= bv-1 (*builtin*.byte-vector 1)))
    (Unit.assert-truthy (= bv-3 (*builtin*.byte-vector 1 2 3)))

    (for-each (list () #t #f 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and) (proc (v)
        (Unit.assert-falsy (= bv-0 v))
    ))
)
