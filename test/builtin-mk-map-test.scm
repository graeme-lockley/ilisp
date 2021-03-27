(import "../lib/unit.scm" :as Unit)

(Unit.test "mk-map"
    (const v1 (*builtin*.mk-map))
    (Unit.assert-equals v1 {})
    (Unit.assert-falsy (*builtin*.mutable? v1))

    (const v2 (*builtin*.mk-map :bob 1 :fred 10 :mary 2))
    (Unit.assert-equals v2 {:bob 1 :fred 10 :mary 2})
    (Unit.assert-falsy (*builtin*.mutable? v2))
)
