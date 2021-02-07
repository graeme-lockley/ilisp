(import "./unit.scm" :as Unit)

(Unit.test "and"
  (Unit.assert-equals (and) 1)
  (Unit.assert-equals (and 1) 1)
  (Unit.assert-equals (and ()) ())
  (Unit.assert-equals (and 1 1) 1)
  (Unit.assert-equals (and 1 ()) ())
  (Unit.assert-equals (and () (/ 1 0)) ())
)

(Unit.test "or"
  (Unit.assert-equals (or) ())
  (Unit.assert-equals (or 1) 1)
  (Unit.assert-equals (or ()) ())
  (Unit.assert-equals (or 1 (/ 1 0)) 1)
  (Unit.assert-equals (or () 1) 1)
  (Unit.assert-equals (or () ()) ())
)
