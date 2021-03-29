(import "../lib/unit.scm" :as Unit)

(Unit.test "implicit apply keyword"
    (Unit.assert-equals :bob :bob)
    (Unit.assert-equals (:bob) :bob)
    (Unit.assert-equals (((:bob))) :bob)

    (Unit.assert-signal (:bob 1 2) (proc (signal) (do
        (Unit.assert-equals (*builtin*.pair-car signal) 'ExpectedArgumentCount)
    )))
)

(Unit.test "explicit apply keyword"
    (Unit.assert-equals (*builtin*.apply :bob '()) :bob)

    (Unit.assert-signal (*builtin*.apply :bob '(1 2)) (proc (signal) (do
        (Unit.assert-equals (*builtin*.pair-car signal) 'ExpectedArgumentCount)
    )))
)
