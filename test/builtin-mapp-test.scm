(import "../lib/unit.scm" :as Unit)

(Unit.test "map?"
    (Unit.assert-falsy (*builtin*.map? ()))
    (Unit.assert-falsy (*builtin*.map? 1))
    (Unit.assert-falsy (*builtin*.map? #\A))
    (Unit.assert-falsy (*builtin*.map? "name"))
    (Unit.assert-falsy (*builtin*.map? 'name))
    (Unit.assert-falsy (*builtin*.map? :name))
    (Unit.assert-falsy (*builtin*.map? '(1 2 3)))
    (Unit.assert-falsy (*builtin*.map? []))
    (Unit.assert-falsy (*builtin*.map? [1 2 3]))
    (Unit.assert-truthy (*builtin*.map? {}))
    (Unit.assert-truthy (*builtin*.map? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.map? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.map? *builtin*.pair-car))
    (Unit.assert-falsy (*builtin*.map? and))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.map?) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.map? 1 2) 'ExpectedArgumentCount)
)
