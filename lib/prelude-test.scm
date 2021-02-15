(import "./unit.scm" :as Unit)

(Unit.test "and"
  (Unit.assert-equals (macroexpand (and)) t)
  (Unit.assert-equals (macroexpand (and A)) 'A)
  (Unit.assert-equals (macroexpand (and A B)) (list 'if 'A (list 'and 'B) ()))
  (Unit.assert-equals (macroexpand (and A B C)) (list 'if 'A (list 'and 'B 'C) ()))

  (Unit.assert-equals (and) t)
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

(Unit.test "count"
  (Unit.assert-equals (count [1 2 3 4]) 4)
  (Unit.assert-equals (count ()) 0)
  (Unit.assert-equals (count '(1 2 3 4 5 6)) 6)
  (Unit.assert-equals (count "hello world") 11)

  (Unit.assert-signal (count 123) (fn (signal) (do
    (Unit.assert-equals (car signal) 'InvalidArgument)
    (Unit.assert-equals (get (cdr signal) :arg-number) 0)
    (Unit.assert-equals (get (cdr signal) :procedure) 'count)
  )))
)

(Unit.test "empty?"
  (Unit.assert-truthy (empty? ()))
  (Unit.assert-falsy (empty? '(1)))

  (Unit.assert-truthy (empty? []))
  (Unit.assert-falsy (empty? [1 2 3]))

  (Unit.assert-truthy (empty? ""))
  (Unit.assert-falsy (empty? "hello"))

  (Unit.assert-signal (empty? 123) (fn (signal) (do
    (Unit.assert-equals (car signal) 'InvalidArgument)
    (Unit.assert-equals (get (cdr signal) :arg-number) 0)
    (Unit.assert-equals (get (cdr signal) :procedure) 'empty?)
  )))
)

(Unit.test "not"
  (Unit.assert-truthy (not f))
  (Unit.assert-truthy (not (= 1 2)))

  (Unit.assert-falsy (not t))
  (Unit.assert-falsy (not (= 1 1)))
)

(Unit.test "or"
  (Unit.assert-equals (macroexpand (or)) ())
  (Unit.assert-equals (macroexpand (or A)) 'A)
  (Unit.assert-equals (macroexpand (or A B)) (list 'if 'A t (list 'or 'B)))
  (Unit.assert-equals (macroexpand (or A B C)) (list 'if 'A t (list 'or 'B 'C)))

  (Unit.assert-equals (or) ())
  (Unit.assert-equals (or 1) 1)
  (Unit.assert-equals (or ()) ())
  (Unit.assert-equals (or 1 (/ 1 0)) t)
  (Unit.assert-equals (or () 1) 1)
  (Unit.assert-equals (or () ()) ())
)
