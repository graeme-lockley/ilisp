(import "./unit.scm" :as Unit)

(Unit.test "and"
  (Unit.assert-equals (macroexpand (and)) #t)
  (Unit.assert-equals (macroexpand (and A)) 'A)
  (Unit.assert-equals (macroexpand (and A B)) (list 'if 'A (list 'and 'B) #f))
  (Unit.assert-equals (macroexpand (and A B C)) (list 'if 'A (list 'and 'B 'C) #f))

  (Unit.assert-equals (and) #t)
  (Unit.assert-equals (and #t) #t)
  (Unit.assert-equals (and #f) #f)
  (Unit.assert-equals (and #t #t) #t)
  (Unit.assert-equals (and #t #f) #f)
  (Unit.assert-equals (and #f (/ 1 0)) #f)
)

(Unit.test "car"
  (Unit.assert-equals (car [1 2 3 4]) 1)
  (Unit.assert-equals (car '(1 2 3 4 5 6)) 1)
  (Unit.assert-equals (car "hello world") 104)

  (Unit.assert-signal (car 123) (proc (signal) (do
    (Unit.assert-equals (car signal) 'InvalidArgument)
    (Unit.assert-equals (get (cdr signal) :arg-number) 0)
    (Unit.assert-equals (get (cdr signal) :procedure) 'car)
  )))
)

(Unit.test "cdr"
  (Unit.assert-equals (cdr [1 2 3 4]) [2 3 4])
  (Unit.assert-equals (cdr '(1 2 3 4 5 6)) '(2 3 4 5 6))
  (Unit.assert-equals (cdr "hello world") "ello world")

  (Unit.assert-signal (cdr 123) (proc (signal) (do
    (Unit.assert-equals (car signal) 'InvalidArgument)
    (Unit.assert-equals (get (cdr signal) :arg-number) 0)
    (Unit.assert-equals (get (cdr signal) :procedure) 'cdr)
  )))
)

(Unit.test "first"
  (Unit.assert-equals (first [1 2 3 4]) 1)
  (Unit.assert-equals (first '(1 2 3 4 5 6)) 1)
  (Unit.assert-equals (first "hello world") 104)
  (Unit.assert-equals (first 123) ())
)

(Unit.test "not"
  (Unit.assert-truthy (not #f))
  (Unit.assert-truthy (not (= 1 2)))

  (Unit.assert-falsy (not #t))
  (Unit.assert-falsy (not (= 1 1)))
)

(Unit.test "or"
  (Unit.assert-equals (macroexpand (or)) #t)
  (Unit.assert-equals (macroexpand (or A)) 'A)
  (Unit.assert-equals (macroexpand (or A B)) (list 'if 'A #t (list 'or 'B)))
  (Unit.assert-equals (macroexpand (or A B C)) (list 'if 'A #t (list 'or 'B 'C)))

  (Unit.assert-equals (or) #t)
  (Unit.assert-equals (or #t) #t)
  (Unit.assert-equals (or #f) #f)
  (Unit.assert-equals (or #t (/ 1 0)) #t)
  (Unit.assert-equals (or #f #t) #t)
  (Unit.assert-equals (or #f #f) #f)
)

(Unit.test "rest"
  (Unit.assert-equals (rest [1 2 3 4]) [2 3 4])
  (Unit.assert-equals (rest '(1 2 3 4 5 6)) '(2 3 4 5 6))
  (Unit.assert-equals (rest "hello world") "ello world")
  (Unit.assert-equals (rest 123) ())
)
