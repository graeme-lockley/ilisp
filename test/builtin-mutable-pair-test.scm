(import "../lib/unit.scm" :as Unit)

(Unit.test "pair"
    (for-each (list () #t #f 1 #\Q 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) car and) 
        (proc (car') 
            (for-each (list () #t #f 1 #\Q 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) car and) 
                (proc (cdr')
                    (const v (*builtin*.mutable-pair car' cdr'))
                    (Unit.assert-equals (car v) car')
                    (Unit.assert-equals (cdr v) cdr')

                    (Unit.assert-truthy (*builtin*.mutable? v))
                )
            )
        )
    )
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.mutable-pair) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.mutable-pair 1) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.mutable-pair 1 2 3) 'ExpectedArgumentCount)
)
