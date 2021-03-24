(import "../unit.scm" :as Unit)
(import "./frame.scm" :as Frame)

(Unit.test "binding"
    (Unit.assert-equals (Frame.binding (Frame.mk () ()) "x") ())

    (Unit.assert-equals (Frame.binding (Frame.mk (list "x") (list 10)) "x") (*builtin*.cons "x" 10))
)
