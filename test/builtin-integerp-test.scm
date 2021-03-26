(import "../lib/unit.scm" :as Unit)

(Unit.test "integer?"
    (Unit.assert-truthy (*builtin*.integer? 1))

    (Unit.assert-falsy (*builtin*.integer? (*builtin*.atom 123)))
    (Unit.assert-falsy (*builtin*.integer? ()))
    (Unit.assert-falsy (*builtin*.integer? #t))
    (Unit.assert-falsy (*builtin*.integer? #f))
    (Unit.assert-falsy (*builtin*.integer? #\A))
    (Unit.assert-falsy (*builtin*.integer? "name"))
    (Unit.assert-falsy (*builtin*.integer? 'name))
    (Unit.assert-falsy (*builtin*.integer? :name))
    (Unit.assert-falsy (*builtin*.integer? '(1 2 3)))
    (Unit.assert-falsy (*builtin*.integer? []))
    (Unit.assert-falsy (*builtin*.integer? [1 2 3]))
    (Unit.assert-falsy (*builtin*.integer? {}))
    (Unit.assert-falsy (*builtin*.integer? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.integer? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.integer? car))
    (Unit.assert-falsy (*builtin*.integer? and))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.integer?) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.integer? 1 2) 'ExpectedArgumentCount)
)
