(import "../lib/unit.scm" :as Unit)

(Unit.test "implicit apply keyword"
    (Unit.assert-equals :bob :bob)
    (Unit.assert-equals (:bob) :bob)
    (Unit.assert-equals (((:bob))) :bob)

    (Unit.assert-signal (:bob 1 2) (proc (signal) (do
        (Unit.assert-equals (car signal) 'ExpectedArgumentCount)
    )))
)

(Unit.test "explicit apply keyword"
    (Unit.assert-equals (apply :bob '()) :bob)

    (Unit.assert-signal (apply :bob '(1 2)) (proc (signal) (do
        (Unit.assert-equals (car signal) 'ExpectedArgumentCount)
    )))
)
