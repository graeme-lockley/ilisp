(import "./unit.scm" :as Unit)

(Unit.test "and"
  (Unit.assert-equals (and) 1)
  (Unit.assert-equals (and 1) 1)
  (Unit.assert-equals (and ()) ())
  (Unit.assert-equals (and 1 1) 1)
  (Unit.assert-equals (and 1 ()) ())
  (Unit.assert-equals (and () (/ 1 0)) ())
)

(Unit.test "cond"
  (Unit.assert-equals (macroexpand (cond)) ())
  (Unit.assert-equals (macroexpand (cond X Y)) (list 'if 'X 'Y (list 'cond)))
  (Unit.assert-equals (macroexpand (cond A B C D)) (list 'if 'A 'B (list 'cond 'C 'D)))
  
  (Unit.assert-equals (cond) ())
  (Unit.assert-equals (cond t 7) 7)
  (Unit.assert-equals (cond f 7) ())
  (Unit.assert-equals (cond t 7 f 8) 7)
  (Unit.assert-equals (cond f 7 t 8) 8)
  (Unit.assert-equals (cond f 7 f 8 :else 9) 9)
  (Unit.assert-equals (cond f 7 f 8 f 9) ())
)

(Unit.test "or"
  (Unit.assert-equals (or) ())
  (Unit.assert-equals (or 1) 1)
  (Unit.assert-equals (or ()) ())
  (Unit.assert-equals (or 1 (/ 1 0)) 1)
  (Unit.assert-equals (or () 1) 1)
  (Unit.assert-equals (or () ()) ())
)
