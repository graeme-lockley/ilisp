(import "../lib/unit.scm" :as Unit)

(define set-car! (get (car **root**) :builtins 'set-car!))

(Unit.test "set-car!"
    (Unit.assert-equals (set-car! (mcons 1 2) 3) (cons 3 2))
)

(Unit.test "set-car! on immutable pair signal"
    (Unit.assert-signal-name (set-car! (cons 1 2) 3) 'ValueIsImmutable)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (set-car!) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (set-car! (mcons 1 2)) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (set-car! (mcons 1 2) 10 "asd") 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (Unit.assert-signal-name (set-car! 'hello 10) 'InvalidArgument)
)
