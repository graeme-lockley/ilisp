(import "./unit.scm" :as Unit)
(import "./vector.scm" :as Vector)

(define (even? n) (= (* (/ n 2) 2) n))

(Unit.test "->mutable"
    (Unit.assert-equals (Vector.->mutable []) [])
    (Unit.assert-truthy (mutable? (Vector.->mutable [])))
)

(Unit.test "car"
    (Unit.assert-equals (car [1]) 1)
    (Unit.assert-equals (car [1 2 3]) 1)

    (Unit.assert-signal (car []) (proc (signal) (do
        (Unit.assert-equals (car signal) 'InvalidArgument)
        (Unit.assert-equals (get (cdr signal) :arg-number) 0)
        (Unit.assert-equals (get (cdr signal) :procedure) 'car)
    )))
)

(Unit.test "cdr"
    (Unit.assert-equals (cdr [1]) [])
    (Unit.assert-equals (cdr [1 2 3]) [2 3])

    (Unit.assert-signal (cdr []) (proc (signal) (do
        (Unit.assert-equals (car signal) 'InvalidArgument)
        (Unit.assert-equals (get (cdr signal) :arg-number) 0)
        (Unit.assert-equals (get (cdr signal) :procedure) 'cdr)
    )))
)

(Unit.test "count"
    (Unit.assert-equals (Vector.count []) 0)
    (Unit.assert-equals (Vector.count [1]) 1)
    (Unit.assert-equals (Vector.count [1 1 1 1 1 1]) 6)
)

(Unit.test "drop"
    (Unit.assert-equals (Vector.drop [] 2) [])
    (Unit.assert-equals (Vector.drop [0 1 2 3 4 5 6 7] 2) [2 3 4 5 6 7])
    (Unit.assert-equals (Vector.drop [0 1 2 3 4 5 6 7] (- 3)) [0 1 2 3 4 5 6 7])
    (Unit.assert-equals (Vector.drop [0 1 2] 10) [])
)

(Unit.test "drop-right"
    (Unit.assert-equals (Vector.drop-right [] 2) [])
    (Unit.assert-equals (Vector.drop-right [0 1 2 3 4 5 6 7] 2) [0 1 2 3 4 5])
    (Unit.assert-equals (Vector.drop-right [0 1 2 3 4 5 6 7] (- 3)) [0 1 2 3 4 5 6 7])
    (Unit.assert-equals (Vector.drop-right [0 1 2] 10) [])
)

(Unit.test "ends-with"
    (Unit.assert-truthy (Vector.ends-with [1 2 3] []))
    (Unit.assert-truthy (Vector.ends-with [1 2 3] [2 3]))
    (Unit.assert-truthy (Vector.ends-with [1 2 3] [1 2 3]))

    (Unit.assert-falsy (Vector.ends-with [1 2 3] [1]))
    (Unit.assert-falsy (Vector.ends-with [1 2 3] [3 3]))
    (Unit.assert-falsy (Vector.ends-with [1 2] [0 1 2]))
)

(Unit.test "filter"
    (Unit.assert-equals (Vector.filter [] even?) [])
    (Unit.assert-equals (Vector.filter [1 3 5 7 9 11] even?) [])
    (Unit.assert-equals (Vector.filter [1 2 3 4 5] even?) [2 4])
    (Unit.assert-equals (Vector.filter [2 4 6 8 10 12] even?) [2 4 6 8 10 12])
)

(Unit.test "first"
    (Unit.assert-equals (first [1]) 1)
    (Unit.assert-equals (first [1 2 3]) 1)
    (Unit.assert-equals (first []) ())
)

(Unit.test "fold"
    (define (icons a b) (cons b a))

    (Unit.assert-equals (Vector.fold [] () icons) '())
    (Unit.assert-equals (Vector.fold [1] () icons) '(1))
    (Unit.assert-equals (Vector.fold [1 2 3 4 5] () icons) '(5 4 3 2 1))

   (Unit.assert-equals (Vector.fold [1 2 3 4 5] "0" (proc (a v) (str "(" a " + " v ")"))) "(((((0 + 1) + 2) + 3) + 4) + 5)")
)

(Unit.test "fold-right"
   (Unit.assert-equals (Vector.fold-right [] () cons) '())
   (Unit.assert-equals (Vector.fold-right [1] () cons) '(1))
   (Unit.assert-equals (Vector.fold-right [1 2 3 4 5] () cons) '(1 2 3 4 5))

   (Unit.assert-equals (Vector.fold-right [1 2 3 4 5] "0" (proc (v a) (str "(" v " + " a ")"))) "(1 + (2 + (3 + (4 + (5 + 0)))))")
)

(Unit.test "nth"
    (Unit.assert-equals (Vector.nth [] 5) ())
    (Unit.assert-equals (Vector.nth [1 2 3] (- 1)) ())
    (Unit.assert-equals (Vector.nth [1] 0) 1)
    (Unit.assert-equals (Vector.nth [1 2] 1) 2)
    (Unit.assert-equals (Vector.nth [1 2] 2) ())
    (Unit.assert-equals (Vector.nth [1 2 () 4 5] 2) ())
)

(Unit.test "nth!"
    (Unit.assert-equals (Vector.nth! (Vector.->mutable [1]) 0 2) [2])
    (Unit.assert-equals (Vector.nth! (Vector.->mutable [1 2]) 1 5) [1 5])
    (Unit.assert-equals (Vector.nth! (Vector.->mutable [1 2 () 4 5]) 2 3) [1 2 3 4 5])

    (Unit.assert-truthy (mutable? (Vector.nth! (Vector.->mutable [1 2 () 4 5]) 2 3)))

    (Unit.assert-signal (Vector.nth! (Vector.->mutable [1]) 2 0) (proc (signal) (do
        (Unit.assert-equals (car signal) 'OutOfRange)
        (Unit.assert-equals (get (cdr signal) :index) 2)
        (Unit.assert-equals (get (cdr signal) :operand) [1])
    )))

    (Unit.assert-signal (Vector.nth! (Vector.->mutable [1]) (- 3) 0) (proc (signal) (do
        (Unit.assert-equals (car signal) 'OutOfRange)
        (Unit.assert-equals (get (cdr signal) :index) (- 3))
        (Unit.assert-equals (get (cdr signal) :operand) [1])
        (Unit.assert-equals (get (cdr signal) :procedure) 'vector-nth!)
    )))

    (Unit.assert-signal (Vector.nth! [1 2 3] 2 0) (proc (signal) (do
        (Unit.assert-equals (car signal) 'ValueIsImmutable)
        (Unit.assert-equals (get (cdr signal) :operand) [1 2 3])
        (Unit.assert-equals (get (cdr signal) :procedure) 'vector-nth!)
    )))
)

(Unit.test "range"
    (Unit.assert-equals (Vector.range 2 2) [2])
    (Unit.assert-equals (Vector.range 0 10) [0 1 2 3 4 5 6 7 8 9 10])
    (Unit.assert-equals (Vector.range 5 2) [5 4 3 2])
)

(Unit.test "rest"
    (Unit.assert-equals (rest [1]) [])
    (Unit.assert-equals (rest [1 2 3]) [2 3])
    (Unit.assert-equals (rest []) ())
)

(Unit.test "reverse"
    (Unit.assert-equals (Vector.reverse []) [])
    (Unit.assert-equals (Vector.reverse [1 2 3 4 5]) [5 4 3 2 1])
)

(Unit.test "slice"
    (Unit.assert-equals (Vector.slice [] 0 10) [])
    (Unit.assert-equals (Vector.slice [0 1 2 3 4 5 6 7] 2 5) [2 3 4 5])
    (Unit.assert-equals (Vector.slice [0 1 2 3 4 5 6 7] 6 10) [6 7])
    (Unit.assert-equals (Vector.slice [0 1 2 3 4 5 6 7] (- 10) 2) [0 1 2])
    (Unit.assert-equals (Vector.slice [0 1 2 3 4 5 6 7] 5 2) [])
)

(Unit.test "sort!"
    (Unit.assert-equals (Vector.sort! (Vector.->mutable [1 0 9 2 3 8 7 4 5 6])) (Vector.range 0 9))
)

(Unit.test "starts-with"
    (Unit.assert-truthy (Vector.starts-with [1 2 3] []))
    (Unit.assert-truthy (Vector.starts-with [1 2 3] [1 2]))
    (Unit.assert-truthy (Vector.starts-with [1 2 3] [1 2 3]))

    (Unit.assert-falsy (Vector.starts-with [1 2 3] [3]))
    (Unit.assert-falsy (Vector.starts-with [1 2] [1 2 3]))
)

(Unit.test "take"
    (Unit.assert-equals (Vector.take [] 2) [])
    (Unit.assert-equals (Vector.take [0 1 2 3 4 5 6 7] 2) [0 1])
    (Unit.assert-equals (Vector.take [0 1 2 3 4 5 6 7] (- 3)) [])
    (Unit.assert-equals (Vector.take [0 1 2] 10) [0 1 2])
)

(Unit.test "take-right"
    (Unit.assert-equals (Vector.take-right [] 2) [])
    (Unit.assert-equals (Vector.take-right [0 1 2 3 4 5 6 7] 2) [6 7])
    (Unit.assert-equals (Vector.take-right [0 1 2 3 4 5 6 7] (- 3)) [])
    (Unit.assert-equals (Vector.take-right [0 1 2] 10) [0 1 2])
)
