(import "../lib/unit.scm" :as Unit)

(Unit.test "mutable?"
    (Unit.assert-falsy (*builtin*.mutable? ()))
    (Unit.assert-falsy (*builtin*.mutable? 1))
    (Unit.assert-falsy (*builtin*.mutable? #\A))
    (Unit.assert-falsy (*builtin*.mutable? "name"))
    (Unit.assert-falsy (*builtin*.mutable? 'name))
    (Unit.assert-falsy (*builtin*.mutable? :name))
    (Unit.assert-falsy (*builtin*.mutable? '(1 2 3)))
    (Unit.assert-falsy (*builtin*.mutable? []))
    (Unit.assert-falsy (*builtin*.mutable? [1 2 3]))
    (Unit.assert-falsy (*builtin*.mutable? {}))
    (Unit.assert-falsy (*builtin*.mutable? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.mutable? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.mutable? car))
    (Unit.assert-falsy (*builtin*.mutable? and))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.mutable?) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.mutable? 1 2) 'ExpectedArgumentCount)
)
