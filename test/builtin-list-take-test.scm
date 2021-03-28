(import "../lib/unit.scm" :as Unit)

(Unit.test "list-take"
    (Unit.assert-equals (*builtin*.list-take () 2) ())
    (Unit.assert-equals (*builtin*.list-take '(0 1 2 3 4 5 6 7) 2) '(0 1))
    (Unit.assert-equals (*builtin*.list-take '(0 1 2 3 4 5 6 7) (- 3)) ())
    (Unit.assert-equals (*builtin*.list-take '(0 1 2) 10) '(0 1 2))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.list-take) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.list-take 1 2 3) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list #t #f 1 #\Q 'hello "hello" :name [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.list-take v 2) 'InvalidArgument)
        )
    )

    (const l (list 1 2 3))

    (for-each (list #t #f () '(1 2 3) #\Q 'hello "hello" :name [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.list-take l v) 'InvalidArgument)
        )
    )
)
