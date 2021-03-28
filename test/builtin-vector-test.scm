(import "../lib/unit.scm" :as Unit)

(Unit.test "vector"
    (const v1 (*builtin*.vector))
    (Unit.assert-equals v1 [])
    (Unit.assert-falsy (*builtin*.mutable? v1))

    (const v2 (*builtin*.vector 1))
    (Unit.assert-equals v2 [1])
    (Unit.assert-falsy (*builtin*.mutable? v2))

    (const v3 (*builtin*.vector 1 2 3))
    (Unit.assert-equals v3 [1 2 3])
    (Unit.assert-falsy (*builtin*.mutable? v3))
)
