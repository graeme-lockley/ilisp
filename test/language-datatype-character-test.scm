(import "../lib/unit.scm" :as Unit)

(Unit.test "printer"
    (Unit.assert-equals (str #\A) "A")
    (Unit.assert-equals (str #x20;) " ")
    (Unit.assert-equals (pr-str #\A) "#\\A")
    (Unit.assert-equals (pr-str #x20) "#x20")
)

(Unit.test "+"
    (Unit.assert-equals (+ #x20; 1) 33)
)

(Unit.test "-"
    (Unit.assert-equals (- #x20;) (- 32))
    (Unit.assert-equals (- #x20; 1) 31)
    (Unit.assert-equals (- 1 #x20;) (- 31))
)

(Unit.test "*"
    (Unit.assert-equals (* #x20; 2) 64)
)

(Unit.test "/"
    (Unit.assert-equals (/ #x20;) 0)
    (Unit.assert-equals (/ #x20; 2) 16)
    (Unit.assert-equals (/ 64 #x20;) 2)
)

(Unit.test "<"
    (Unit.assert-falsy (< #x20; 31))
    (Unit.assert-falsy (< #x20; 32))
    (Unit.assert-truthy (< #x20; 33))

    (Unit.assert-truthy (< 31 #x20;))
    (Unit.assert-falsy (< 32 #x20;))
    (Unit.assert-falsy (< 33 #x20;))
)

(Unit.test "<="
    (Unit.assert-falsy (<= #x20; 31))
    (Unit.assert-truthy (<= #x20; 32))
    (Unit.assert-truthy (<= #x20; 33))

    (Unit.assert-truthy (<= 31 #x20;))
    (Unit.assert-truthy (<= 32 #x20;))
    (Unit.assert-falsy (<= 33 #x20;))
)

(Unit.test ">"
    (Unit.assert-truthy (> #x20; 31))
    (Unit.assert-falsy (> #x20; 32))
    (Unit.assert-falsy (> #x20; 33))

    (Unit.assert-falsy (> 31 #x20;))
    (Unit.assert-falsy (> 32 #x20;))
    (Unit.assert-truthy (> 33 #x20;))
)

(Unit.test ">="
    (Unit.assert-truthy (>= #x20; 31))
    (Unit.assert-truthy (>= #x20; 32))
    (Unit.assert-falsy (>= #x20; 33))

    (Unit.assert-falsy (>= 31 #x20;))
    (Unit.assert-truthy (>= 32 #x20;))
    (Unit.assert-truthy (>= 33 #x20;))
)
