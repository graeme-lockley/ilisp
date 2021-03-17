(import "../lib/unit.scm" :as Unit)

(Unit.test "count"
    (Unit.assert-equals (*builtin*.byte-vector-count (*builtin*.byte-vector)) 0)
    (Unit.assert-equals (*builtin*.byte-vector-count (*builtin*.byte-vector 1)) 1)
    (Unit.assert-equals (*builtin*.byte-vector-count (*builtin*.byte-vector 1 2 3)) 3)
)
