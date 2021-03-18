(import "../lib/unit.scm" :as Unit)

(Unit.test "cons pair is immutable"
    (Unit.assert-falsy (mutable? (cons 1 2)))
    (Unit.assert-falsy (mutable? ()))
    (Unit.assert-falsy (mutable? (list 1 2 3 4 5)))
)

(Unit.test "mcons pair is mutable"
    (Unit.assert-truthy (mutable? (mcons 1 2)))

    (const list' (mcons 1 (list 2 3)))
    (Unit.assert-truthy (mutable? list'))
    (Unit.assert-falsy (mutable? (cdr list')))
)
