(import "../lib/unit.scm" :as Unit)

(Unit.test "atom-swap!"
    (const atom (*builtin*.atom 0))
    (const (nudge) (*builtin*.atom-swap! atom (proc (n) (+ n 1))))

    (Unit.assert-equals (*builtin*.atom-dereference atom) 0)

    (nudge)

    (Unit.assert-equals (*builtin*.atom-dereference atom) 1)

    (nudge)

    (Unit.assert-equals (*builtin*.atom-dereference atom) 2)
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.atom-swap!) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.atom-swap! 1 2 3) 'ExpectedArgumentCount)
)

(Unit.test "arg 0 incorrect type signal"
    (const atom (*builtin*.atom 0))
    (const (inc n) (+ n 1))

    (for-each (list () #t #f 1 #\Q 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) car and) 
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.atom-swap! v inc) 'InvalidArgument)
        )
    )

    (for-each (list () #t #f 1 #\Q 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} atom car and) 
        (proc (v) 
            (Unit.assert-signal-name (*builtin*.atom-swap! atom v) 'InvalidArgument)
        )
    )
)
