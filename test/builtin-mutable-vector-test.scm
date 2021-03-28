(import "../lib/unit.scm" :as Unit)

(Unit.test "mutable-vector"
    (const v1 (*builtin*.mutable-vector))
    (Unit.assert-equals v1 [])
    (Unit.assert-truthy (*builtin*.mutable? v1))

    (const v2 (*builtin*.mutable-vector 1))
    (Unit.assert-equals v2 [1])
    (Unit.assert-truthy (*builtin*.mutable? v2))

    (const v3 (*builtin*.mutable-vector 1 2 3))
    (Unit.assert-equals v3 [1 2 3])
    (Unit.assert-truthy (*builtin*.mutable? v3))
)
