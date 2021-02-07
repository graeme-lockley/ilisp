(import "./unit.scm" :as Unit)
(import "./vector.scm" :as Vector)

(define (even? n) (= (* (/ n 2) 2) n))

(Unit.test "car"
    (Unit.assert-equals (car [1]) 1)
    (Unit.assert-equals (car [1 2 3]) 1)

    (Unit.assert-signal (car []) (fn (signal) (do
        (Unit.assert-equals (car signal) 'InvalidArgument)
        (Unit.assert-equals (get (cdr signal) :arg-number) 0)
        (Unit.assert-equals (get (cdr signal) :procedure) 'car)
    )))
)

(Unit.test "cdr"
    (Unit.assert-equals (cdr [1]) [])
    (Unit.assert-equals (cdr [1 2 3]) [2 3])

    (Unit.assert-signal (cdr []) (fn (signal) (do
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

(Unit.test "nth"
    (Unit.assert-equals (Vector.nth [] 5) ())
    (Unit.assert-equals (Vector.nth [1] 0) 1)
    (Unit.assert-equals (Vector.nth [1 2] 1) 2)
    (Unit.assert-equals (Vector.nth [1 2] 2) ())
    (Unit.assert-equals (Vector.nth [1 2 () 4 5] 2) ())
)

(Unit.test "rest"
    (Unit.assert-equals (rest [1]) [])
    (Unit.assert-equals (rest [1 2 3]) [2 3])
    (Unit.assert-equals (rest []) ())
)

(Unit.test "slice"
    (Unit.assert-equals (Vector.slice [] 0 10) [])
    (Unit.assert-equals (Vector.slice [0 1 2 3 4 5 6 7] 2 5) [2 3 4 5])
    (Unit.assert-equals (Vector.slice [0 1 2 3 4 5 6 7] 6 10) [6 7])
    (Unit.assert-equals (Vector.slice [0 1 2 3 4 5 6 7] (- 10) 2) [0 1 2])
    (Unit.assert-equals (Vector.slice [0 1 2 3 4 5 6 7] 5 2) [])
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
