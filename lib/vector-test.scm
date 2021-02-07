(import "./unit.scm" :as Unit)
(import "./vector.scm" :as Vector)

(define (even? n) (= (* (/ n 2) 2) n))

(Unit.test "filter"
    (Unit.assert-equals (Vector.filter [] even?) [])
    (Unit.assert-equals (Vector.filter [1 3 5 7 9 11] even?) [])
    (Unit.assert-equals (Vector.filter [1 2 3 4 5] even?) [2 4])
    (Unit.assert-equals (Vector.filter [2 4 6 8 10 12] even?) [2 4 6 8 10 12])
)
