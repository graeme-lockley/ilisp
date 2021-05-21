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
    (const (verify text offset line column)
        (const scanner (Scanner.mk-scanner (string->byte-vector text)))

        (Unit.assert-equals (Scanner.Scanner-current-token scanner) (Scanner.Token Scanner.TEOS (Scanner.Coordinate offset line column) ""))
    )

    (verify "" 0 1 1)
    (verify " " 1 1 2)
    (verify "  " 2 1 3)
)
