(import "../lib/unit.scm" :as Unit)

(Unit.test "macro?"
    (Unit.assert-falsy (*builtin*.macro? ()))
    (Unit.assert-falsy (*builtin*.macro? 1))
    (Unit.assert-falsy (*builtin*.macro? #\A))
    (Unit.assert-falsy (*builtin*.macro? "name"))
    (Unit.assert-falsy (*builtin*.macro? 'name))
    (Unit.assert-falsy (*builtin*.macro? :name))
    (Unit.assert-falsy (*builtin*.macro? '(1 2 3)))
    (Unit.assert-falsy (*builtin*.macro? []))
    (Unit.assert-falsy (*builtin*.macro? [1 2 3]))
    (Unit.assert-falsy (*builtin*.macro? {}))
    (Unit.assert-falsy (*builtin*.macro? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.macro? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.macro? *builtin*.pair-car))
    (Unit.assert-truthy (*builtin*.macro? and))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.macro?) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.macro? 1 2) 'ExpectedArgumentCount)
)
