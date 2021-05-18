(import "../byte-vector.scm" :names string->byte-vector)
(import "../unit.scm" :as Unit)

(import "./scanner.scm" :as Scanner)

(Unit.test "Coordinate"
    (const c (Scanner.Coordinate 1 2 3))

    (Unit.assert-truthy (Scanner.Coordinate? c))
    (Unit.assert-equals (Scanner.Coordinate-offset c) 1)
    (Unit.assert-equals (Scanner.Coordinate-line c) 2)
    (Unit.assert-equals (Scanner.Coordinate-column c) 3)
)

(Unit.test "initialise scanner"
    (const scanner (Scanner.mk-scanner (string->byte-vector "  ")))

    ;; (Unit.assert-equals (Scanner.Scanner-current-token scanner) 0)
)