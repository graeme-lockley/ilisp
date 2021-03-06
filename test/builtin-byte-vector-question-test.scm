(import "../lib/unit.scm" :as Unit)

(Unit.test "byte-vector?"
    (Unit.assert-falsy (*builtin*.byte-vector? ()))
    (Unit.assert-falsy (*builtin*.byte-vector? 1))
    (Unit.assert-falsy (*builtin*.byte-vector? #\A))
    (Unit.assert-falsy (*builtin*.byte-vector? "name"))
    (Unit.assert-falsy (*builtin*.byte-vector? 'name))
    (Unit.assert-falsy (*builtin*.byte-vector? :name))
    (Unit.assert-falsy (*builtin*.byte-vector? '(1 2 3)))
    (Unit.assert-falsy (*builtin*.byte-vector? []))
    (Unit.assert-falsy (*builtin*.byte-vector? [1 2 3]))
    (Unit.assert-truthy (*builtin*.byte-vector? (*builtin*.byte-vector 1 2 3)))
    (Unit.assert-falsy (*builtin*.byte-vector? {}))
    (Unit.assert-falsy (*builtin*.byte-vector? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.byte-vector? (*builtin*.atom 1)))
    (Unit.assert-falsy (*builtin*.byte-vector? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.byte-vector? *builtin*.pair-car))
    (Unit.assert-falsy (*builtin*.byte-vector? and))
)
