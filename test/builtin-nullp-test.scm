(import "../lib/unit.scm" :as Unit)

(Unit.test "null?"
    (Unit.assert-truthy (*builtin*.null? ()))
    
    (Unit.assert-falsy (*builtin*.null? (*builtin*.atom 123)))
    (Unit.assert-falsy (*builtin*.null? 1))
    (Unit.assert-falsy (*builtin*.null? #t))
    (Unit.assert-falsy (*builtin*.null? #f))
    (Unit.assert-falsy (*builtin*.null? #\A))
    (Unit.assert-falsy (*builtin*.null? "name"))
    (Unit.assert-falsy (*builtin*.null? 'name))
    (Unit.assert-falsy (*builtin*.null? :name))
    (Unit.assert-falsy (*builtin*.null? '(1 2 3)))
    (Unit.assert-falsy (*builtin*.null? []))
    (Unit.assert-falsy (*builtin*.null? [1 2 3]))
    (Unit.assert-falsy (*builtin*.null? {}))
    (Unit.assert-falsy (*builtin*.null? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.null? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.null? *builtin*.pair-car))
    (Unit.assert-falsy (*builtin*.null? and))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.null? ) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.null? 1 2) 'ExpectedArgumentCount)
)
