(import "../lib/unit.scm" :as Unit)

(Unit.test "string-map"
    (const (inc n) (+ n 1))

    (Unit.assert-equals (*builtin*.string-map "" inc) ())
    (Unit.assert-equals (*builtin*.string-map "h" inc) '(105))
    (Unit.assert-equals (*builtin*.string-map "IBM" inc) '(74 67 78))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.string-map) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string-map 1) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string-map 1 2 3) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (const (inc n) (+ n 1))

    (for-each (list #t #f 1 #\Q 'hello :name () '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.string-map v inc) 'InvalidArgument)
        )
    )

    (for-each (list #t #f () '(1 2 3) #\Q 'hello "hello" :name [] [1 2 3] {} {:a 1 :b 2 :c 3} *builtin*.pair-car and (*builtin*.byte-vector) (*builtin*.byte-vector 1 2 3))
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.list-take "hello" v) 'InvalidArgument)
        )
    )
)
