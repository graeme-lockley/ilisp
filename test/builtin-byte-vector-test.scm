(import "../lib/unit.scm" :as Unit)

(define byte-vector (get (car **root**) :builtins 'byte-vector))
(define byte-vector-count (get (car **root**) :builtins 'byte-vector-count))
(define byte-vector-nth (get (car **root**) :builtins 'byte-vector-nth))

(Unit.test "byte-vector"
    (Unit.assert-equals (byte-vector-count (byte-vector)) 0)

    (define bv' (byte-vector 1 #\2 3))

    (Unit.assert-equals (byte-vector-count bv') 3)

    (Unit.assert-equals (byte-vector-nth bv' 0) 1)
    (Unit.assert-equals (byte-vector-nth bv' 1) 50)
    (Unit.assert-equals (byte-vector-nth bv' 2) 3)

    (Unit.assert-equals (byte-vector-nth bv' (- 1)) 0)
    (Unit.assert-equals (byte-vector-nth bv' 4) 0)
)

(Unit.test "incorrect argument type signal"
    (define inputs
        (list () #t #f 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (fn (n) (+ n 1)) car and)
    )

    (for-each inputs (fn (input)
        (Unit.assert-signal-name (byte-vector 1 2 input 3 4) 'InvalidArgument)
    ))
)
