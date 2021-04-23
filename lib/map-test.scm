(import "./map.scm" :as Map)

(import "./unit.scm" :as Unit)


(Unit.test "union"
    (Unit.assert-equals (Map.union) {})
    (Unit.assert-equals (Map.union {:a 1 :b 2 :c 3}) {:a 1 :b 2 :c 3})
    (Unit.assert-equals (Map.union {:a 1 :b 2 :c 3} {:a 10 :d 4}) {:a 10 :b 2 :c 3 :d 4})
)
