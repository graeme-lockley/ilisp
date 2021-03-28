(import "../lib/unit.scm" :as Unit)

(Unit.test "pair?"
    (Unit.assert-truthy (*builtin*.pair? '(1 2 3)))

    (Unit.assert-falsy (*builtin*.pair? ()))
    (Unit.assert-falsy (*builtin*.pair? 1))
    (Unit.assert-falsy (*builtin*.pair? #\A))
    (Unit.assert-falsy (*builtin*.pair? "name"))
    (Unit.assert-falsy (*builtin*.pair? 'name))
    (Unit.assert-falsy (*builtin*.pair? :name))
    (Unit.assert-falsy (*builtin*.pair? []))
    (Unit.assert-falsy (*builtin*.pair? [1 2 3]))
    (Unit.assert-falsy (*builtin*.pair? {}))
    (Unit.assert-falsy (*builtin*.pair? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.pair? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.pair? *builtin*.pair-car))
    (Unit.assert-falsy (*builtin*.pair? and))
)

(Unit.test "arg count signal"
    (Unit.assert-signal-name (*builtin*.pair? ) 'ExpectedArgumentCount)
    (Unit.assert-signal-name (*builtin*.pair? 1 2) 'ExpectedArgumentCount)
)
