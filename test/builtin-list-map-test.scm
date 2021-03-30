(import "../lib/unit.scm" :as Unit)

(Unit.test "list-map"
    (const (inc n) (+ n 1))

    (Unit.assert-equals (*builtin*.list-map () inc) ())
    (Unit.assert-equals (*builtin*.list-map (list 1) inc) (list 2))
    (Unit.assert-equals (*builtin*.list-map (list 1 2 3) inc) (list 2 3 4))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.list-map) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.list-map 1) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.list-map 1 2 3) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (for-each (list #t #f 1 #\Q 'hello "hello" :name [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.list-map v 2) 'InvalidArgument)
        )
    )

    (const l (list 1 2 3))

    (for-each (list #t #f () '(1 2 3) #\Q 'hello "hello" :name [] [1 2 3] {} {:a 1 :b 2 :c 3} *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.list-take l v) 'InvalidArgument)
        )
    )
)
