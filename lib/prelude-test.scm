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

(Unit.test "ends-with"
  (Unit.assert-truthy (ends-with [1 2 3 4] [3 4]))
  (Unit.assert-falsy (ends-with [1 2 3 4] [2 3]))

  (Unit.assert-truthy (ends-with '(1 2 3 4 5 6) '(4 5 6)))
  (Unit.assert-falsy (ends-with '(1 2 3 4 5 6) '(3 4 5)))

  (Unit.assert-truthy (ends-with "hello world" "world"))
  (Unit.assert-falsy (ends-with "hello world" "worl"))

  (Unit.assert-signal (ends-with 123 1) (fn (signal) (do
    (Unit.assert-equals (car signal) 'InvalidArgument)
    (Unit.assert-equals (get (cdr signal) :arg-number) 0)
    (Unit.assert-equals (get (cdr signal) :procedure) 'ends-with)
  )))
)

(Unit.test "filter"
  (define (odd n) (= (- n (* (/ n 2) 2)) 1))

  (Unit.assert-equals (filter [1 2 3 4] odd) [1 3])
  (Unit.assert-equals (filter '(1 2 3 4 5 6) odd) '(1 3 5))
  (Unit.assert-equals (filter "hello world" odd) "eowo")

  (Unit.assert-signal (filter 123 1) (fn (signal) (do
    (Unit.assert-equals (car signal) 'InvalidArgument)
    (Unit.assert-equals (get (cdr signal) :arg-number) 0)
    (Unit.assert-equals (get (cdr signal) :procedure) 'filter)
  )))
)

(Unit.test "not"
  (Unit.assert-truthy (not f))
  (Unit.assert-truthy (not (= 1 2)))

  (Unit.assert-falsy (not t))
  (Unit.assert-falsy (not (= 1 1)))
)

(Unit.test "nth"
  (Unit.assert-equals (nth [1 2 3 4] 2) 3)
  (Unit.assert-equals (nth '(1 2 3 4 5 6) 2) 3)
  (Unit.assert-equals (nth "hello world" 2) 108)

  (Unit.assert-signal (nth 123 1) (fn (signal) (do
    (Unit.assert-equals (car signal) 'InvalidArgument)
    (Unit.assert-equals (get (cdr signal) :arg-number) 0)
    (Unit.assert-equals (get (cdr signal) :procedure) 'nth)
  )))
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

(Unit.test "slice"
  (Unit.assert-equals (slice [1 2 3 4] 1 2) [2 3])

  (Unit.assert-equals (slice '(1 2 3 4 5 6) 1 2) '(2 3))
  
  (Unit.assert-equals (slice "hello world" 1 2) "el")
  
  (Unit.assert-signal (slice 123 1 2) (fn (signal) (do
    (Unit.assert-equals (car signal) 'InvalidArgument)
    (Unit.assert-equals (get (cdr signal) :arg-number) 0)
    (Unit.assert-equals (get (cdr signal) :procedure) 'slice)
  )))
)

(Unit.test "starts-with"
  (Unit.assert-truthy (starts-with [1 2 3 4] [1 2]))
  (Unit.assert-falsy (starts-with [1 2 3 4] [1 2 4]))

  (Unit.assert-truthy (starts-with '(1 2 3 4 5 6) '(1 2 3)))
  (Unit.assert-falsy (starts-with '(1 2 3 4 5 6) '(1 2 4)))

  (Unit.assert-truthy (starts-with "hello world" "hello"))
  (Unit.assert-falsy (starts-with "hello world" "helos"))

  (Unit.assert-signal (starts-with 123 1) (fn (signal) (do
    (Unit.assert-equals (car signal) 'InvalidArgument)
    (Unit.assert-equals (get (cdr signal) :arg-number) 0)
    (Unit.assert-equals (get (cdr signal) :procedure) 'starts-with)
  )))
)

