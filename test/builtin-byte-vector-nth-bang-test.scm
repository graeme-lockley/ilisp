(import "../lib/unit.scm" :as Unit)

(define byte-vector (get (car **root**) :builtins 'byte-vector))
(define byte-vector-count (get (car **root**) :builtins 'byte-vector-count))
(define byte-vector-mutable (get (car **root**) :builtins 'byte-vector-mutable))
(define byte-vector-nth (get (car **root**) :builtins 'byte-vector-nth))
(define byte-vector-nth! (get (car **root**) :builtins 'byte-vector-nth!))

(Unit.test "byte-vector-nth!"
    (define bv' (byte-vector-mutable (byte-vector 1 #\2 3)))

    (Unit.assert-equals (byte-vector-nth bv' 0) 1)
    (Unit.assert-equals (byte-vector-nth bv' 1) 50)
    (Unit.assert-equals (byte-vector-nth bv' 2) 3)

    (byte-vector-nth! bv' 1 100)

    (Unit.assert-equals (byte-vector-nth bv' 1) 100)
)

(Unit.test "out of range index signal"
    (define bv' (byte-vector-mutable (byte-vector 1 #\2 3)))

    (Unit.assert-signal-name (byte-vector-nth! bv' (- 1) 100) 'OutOfRange)
    (Unit.assert-signal-name (byte-vector-nth! bv' 3 100) 'OutOfRange)
)

(Unit.test "arg 0 incorrect type signal"
    (define inputs (list () #t #f 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (fn (n) (+ n 1)) car and))

    (for-each inputs (fn (input)
        (Unit.assert-signal-name (byte-vector-nth! input 0 0) 'InvalidArgument)
    ))
)

(Unit.test "arg 1 incorrect type signal"
    (define bv' (byte-vector-mutable (byte-vector 1 #\2 3)))
    (define inputs (list () #t #f 'hello "hello" #\a :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (fn (n) (+ n 1)) car and))

    (for-each inputs (fn (input)
        (Unit.assert-signal-name (byte-vector-nth! bv' input 0) 'InvalidArgument)
    ))
)

(Unit.test "argument count signal"
    (define bv' (byte-vector-mutable (byte-vector 1 #\2 3)))

    (Unit.assert-signal-name (byte-vector-nth!) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (byte-vector-nth! bv') 'ExpectedArgumentCount)
    (Unit.assert-signal-name (byte-vector-nth! bv' 1) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (byte-vector-nth! bv' 1 0 2) 'ExpectedArgumentCount)
)
