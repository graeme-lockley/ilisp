(import "../lib/unit.scm" :as Unit)

(define string-nth (get (car **root**) :builtins 'string-nth))

(Unit.test "character?"
    (Unit.assert-truthy (character? (string-nth "Hello world" 0)))
)

(Unit.test "out of range"
    (Unit.assert-equals (string-nth "Hello world" (- 1)) #x00)
    (Unit.assert-equals (string-nth "Hello world" 100) #x00)
)

(Unit.test "nth"
    (Unit.assert-equals (string-nth "hello" 1) #\e)
    (Unit.assert-equals (string-nth "hello" 2) #\l)
    (Unit.assert-equals (string-nth "hello" (- 1)) #x00)
    (Unit.assert-equals (string-nth "hello" 10) #x00) 
    (Unit.assert-equals (string-nth "" 0) #x00)
)

(Unit.test "arg count"
    (define (assert-argument-count s)
        (Unit.assert-equals (car s) 'ExpectedArgumentCount)
    )

    (define (assert-invalid-argument s)
        (Unit.assert-equals (car s) 'InvalidArgument)
    )

    (Unit.assert-signal (string-nth) assert-argument-count)
    (Unit.assert-signal (string-nth "hello") assert-argument-count)
    (Unit.assert-signal (string-nth "hello" 10 "asd") assert-argument-count)

    (Unit.assert-signal (string-nth 'hello 10) assert-invalid-argument)
    (Unit.assert-signal (string-nth "hello" #x00) assert-invalid-argument)
)
