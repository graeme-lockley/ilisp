(import "../lib/unit.scm" :as Unit)

(Unit.test "string?"
    (Unit.assert-truthy (*builtin*.string? "name"))

    (Unit.assert-falsy (*builtin*.string? ()))
    (Unit.assert-falsy (*builtin*.string? 1))
    (Unit.assert-falsy (*builtin*.string? #\A))
    (Unit.assert-falsy (*builtin*.string? 'name))
    (Unit.assert-falsy (*builtin*.string? :name))
    (Unit.assert-falsy (*builtin*.string? '(1 2 3)))
    (Unit.assert-falsy (*builtin*.string? []))
    (Unit.assert-falsy (*builtin*.string? [1 2 3]))
    (Unit.assert-falsy (*builtin*.string? {}))
    (Unit.assert-falsy (*builtin*.string? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.string? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.string? *builtin*.pair-car))
    (Unit.assert-falsy (*builtin*.string? and))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.string? ) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.string? 1 2) 'ExpectedArgumentCount)
)
