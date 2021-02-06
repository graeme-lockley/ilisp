(import "./list.scm" :as List)

(import "./unit.scm" :as Unit)

(define (even? n) (= (* (/ n 2) 2) n))

(Unit.assert-equals (List.filter even? '()) '())
(Unit.assert-equals (List.filter even? '(1 3 5 7 9 11)) '())
(Unit.assert-equals (List.filter even? '(1 2 3 4 5)) '(2 4))
(Unit.assert-equals (List.filter even? '(2 4 6 8 10 12)) '(2 4 6 8 10 12))

; (Unit.assert-equals (List.filter even? [2 4 6 8 10 12]) '(2 4 6 8 10 12))


