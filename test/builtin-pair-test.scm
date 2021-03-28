(import "../lib/unit.scm" :as Unit)

(Unit.test "pair"
    (for-each (list () #t #f 1 #\Q 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) car and) 
        (proc (car') 
            (for-each (list () #t #f 1 #\Q 'hello "hello" :name '(1 2 3) [] [1 2 3] {} {:a 1 :b 2 :c 3} (proc (n) (+ n 1)) car and) 
                (proc (cdr')
                    (Unit.assert-equals (car (*builtin*.pair car' cdr')) car')
                    (Unit.assert-equals (cdr (*builtin*.pair car' cdr')) cdr')
                )
            )
        )
    )
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.pair) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.pair 1) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.atom 1 2 3) 'ExpectedArgumentCount)
)
