(import "../lib/unit.scm" :as Unit)

(Unit.test "number?"
    (Unit.assert-truthy (*builtin*.number? 1))

    (Unit.assert-falsy (*builtin*.number? (*builtin*.atom 123)))
    (Unit.assert-falsy (*builtin*.number? ()))
    (Unit.assert-falsy (*builtin*.number? #t))
    (Unit.assert-falsy (*builtin*.number? #f))
    (Unit.assert-falsy (*builtin*.number? #\A))
    (Unit.assert-falsy (*builtin*.number? "name"))
    (Unit.assert-falsy (*builtin*.number? 'name))
    (Unit.assert-falsy (*builtin*.number? :name))
    (Unit.assert-falsy (*builtin*.number? '(1 2 3)))
    (Unit.assert-falsy (*builtin*.number? []))
    (Unit.assert-falsy (*builtin*.number? [1 2 3]))
    (Unit.assert-falsy (*builtin*.number? {}))
    (Unit.assert-falsy (*builtin*.number? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.number? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.number? car))
    (Unit.assert-falsy (*builtin*.number? and))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.number?) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.number? 1 2) 'ExpectedArgumentCount)
)
