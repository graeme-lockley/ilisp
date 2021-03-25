(import "../lib/unit.scm" :as Unit)

(Unit.test "printer"
    (Unit.assert-equals (str #f) "#f")
)