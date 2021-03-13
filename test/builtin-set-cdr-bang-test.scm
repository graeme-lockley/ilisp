(import "../lib/unit.scm" :as Unit)

(define set-cdr! (get (car **root**) :builtins 'set-cdr!))

(Unit.test "set-cdr!"
    (Unit.assert-equals (set-cdr! (mcons 1 2) 3) (cons 1 3))
)

(Unit.test "set-cdr! on immutable pair signal"
    (Unit.assert-signal-name (set-cdr! (cons 1 2) 3) 'ValueIsImmutable)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (set-cdr!) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (set-cdr! (mcons 1 2)) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (set-cdr! (mcons 1 2) 10 "asd") 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (Unit.assert-signal-name (set-cdr! 'hello 10) 'InvalidArgument)
)
