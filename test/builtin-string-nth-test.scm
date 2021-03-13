(import "../lib/unit.scm" :as Unit)

(define string-nth (get (car **root**) :builtins 'string-nth))

(Unit.test "nth"
    (Unit.assert-equals (string-nth "hello" 1) #\e)
    (Unit.assert-equals (string-nth "hello" 2) #\l)
    (Unit.assert-equals (string-nth "hello" (- 1)) #x00)
    (Unit.assert-equals (string-nth "hello" 10) #x00) 
    (Unit.assert-equals (string-nth "" 0) #x00)
)

(Unit.test "result is character"
    (Unit.assert-truthy (character? (string-nth "Hello world" 0)))
)

(Unit.test "out of range index"
    (Unit.assert-equals (string-nth "Hello world" (- 1)) #x00)
    (Unit.assert-equals (string-nth "Hello world" 100) #x00)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (string-nth) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (string-nth "hello") 'ExpectedArgumentCount)
    (Unit.assert-signal-name (string-nth "hello" 10 "asd") 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (Unit.assert-signal-name (string-nth 'hello 10) 'InvalidArgument)
)

(Unit.test "arg 1 incorrect type signal"
    (Unit.assert-signal-name (string-nth "hello" #x00) 'InvalidArgument)
)
