(import "../lib/unit.scm" :as Unit)

(Unit.test "character?"
    (Unit.assert-falsy (*builtin*.character? ()))
    (Unit.assert-falsy (*builtin*.character? #t))
    (Unit.assert-falsy (*builtin*.character? #f))
    (Unit.assert-truthy (*builtin*.character? #\A))
    (Unit.assert-falsy (*builtin*.character? 1))
    (Unit.assert-falsy (*builtin*.character? "name"))
    (Unit.assert-falsy (*builtin*.character? 'name))
    (Unit.assert-falsy (*builtin*.character? :name))
    (Unit.assert-falsy (*builtin*.character? '(1 2 3)))
    (Unit.assert-falsy (*builtin*.character? []))
    (Unit.assert-falsy (*builtin*.character? [1 2 3]))
    (Unit.assert-falsy (*builtin*.character? {}))
    (Unit.assert-falsy (*builtin*.character? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.character? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.character? *builtin*.pair-car))
    (Unit.assert-falsy (*builtin*.character? and))
)
