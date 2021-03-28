(import "../lib/unit.scm" :as Unit)

(Unit.test "cons pair is immutable"
    (Unit.assert-falsy (*builtin*.mutable? (*builtin*.pair 1 2)))
    (Unit.assert-falsy (*builtin*.mutable? ()))
    (Unit.assert-falsy (*builtin*.mutable? (list 1 2 3 4 5)))
)

(Unit.test "mcons pair is mutable"
    (Unit.assert-truthy (*builtin*.mutable? (mcons 1 2)))

    (const list' (mcons 1 (list 2 3)))
    (Unit.assert-truthy (*builtin*.mutable? list'))
    (Unit.assert-falsy (*builtin*.mutable? (cdr list')))
)
