(import "./list.scm" :as List)

(import "./unit.scm" :as Unit)

(define (even? n) (= (* (/ n 2) 2) n))

(Unit.test "filter"
    (Unit.assert-equals (List.filter '() even?) '())
    (Unit.assert-equals (List.filter '(1 3 5 7 9 11) even?) '())
    (Unit.assert-equals (List.filter '(1 2 3 4 5) even?) '(2 4))
    (Unit.assert-equals (List.filter '(2 4 6 8 10 12) even?) '(2 4 6 8 10 12))

    (Unit.assert-signal (List.filter [2 4 6 8 10 12] even?) (fn (signal) (do
        (Unit.assert-equals (car signal) 'InvalidArgument)
        (Unit.assert-equals (get (cdr signal) :arg-number) 0)
        (Unit.assert-equals (get (cdr signal) :procedure) 'list-filter)
    )))
)
