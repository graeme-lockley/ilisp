(import "../lib/unit.scm" :as Unit)

(define byte-vector (get (car **root**) :builtins 'byte-vector))

(Unit.test "byte-vector ="
    (define bv-0 (byte-vector))
    (define bv-1 (byte-vector 1))
    (define bv-3 (byte-vector 1 2 3))

    (Unit.assert-truthy (= bv-0 (byte-vector)))
    (Unit.assert-truthy (= bv-1 (byte-vector 1)))
    (Unit.assert-truthy (= bv-3 (byte-vector 1 2 3)))

    (for-each (list () #t #f 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) car and) (proc (v)
        (Unit.assert-falsy (= bv-0 v))
    ))
)
