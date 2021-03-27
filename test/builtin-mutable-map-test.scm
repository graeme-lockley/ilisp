(import "../lib/unit.scm" :as Unit)

(Unit.test "mutable-map"
    (const v1 (*builtin*.mutable-map))
    (Unit.assert-equals v1 {})
    (Unit.assert-truthy (*builtin*.mutable? v1))

    (const v2 (*builtin*.mutable-map :bob 1 :fred 10 :mary 2))
    (Unit.assert-equals v2 {:bob 1 :fred 10 :mary 2})
    (Unit.assert-truthy (*builtin*.mutable? v2))
)
