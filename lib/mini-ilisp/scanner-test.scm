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

(const (tokens scanner)
    (const token (Scanner.Scanner-current-token scanner))

    ;; (println "tokens: " (pr-str token))

    (if (= (Scanner.Token-token token) Scanner.TEOS) (list token)
        (do (Scanner.next scanner)
            (pair token (tokens scanner))
        )
    )
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

(Unit.test "match identifier"
    (const scanner (Scanner.mk-scanner (string->byte-vector "a hello symbol-name?")))
    (const ts (tokens scanner))

    (const t-a (nth ts 0))
    (const t-hello (nth ts 1))
    (const t-symbol-name? (nth ts 2))
    (const t-eos (nth ts 3))

    ;; (println (pr-str ts))

    (Unit.assert-equals (count ts) 4)

    (Unit.assert-equals (Scanner.Token-token t-a) Scanner.TIdentifier)
    (Unit.assert-truthy (Scanner.Coordinate? (Scanner.Token-location t-a)))
    (Unit.assert-equals (Scanner.Coordinate-offset (Scanner.Token-location t-a)) 0)
    (Unit.assert-equals (Scanner.Token-lexeme t-a) "a")

    (Unit.assert-equals (Scanner.Token-token t-hello) Scanner.TIdentifier)
    (Unit.assert-truthy (Scanner.Range? (Scanner.Token-location t-hello)))
    (Unit.assert-equals (Scanner.Coordinate-offset (Scanner.Range-start (Scanner.Token-location t-hello))) 2)
    (Unit.assert-equals (Scanner.Coordinate-offset (Scanner.Range-end (Scanner.Token-location t-hello))) 6)
    (Unit.assert-equals (Scanner.Token-lexeme t-hello) "hello")

    (Unit.assert-equals (Scanner.Token-token t-symbol-name?) Scanner.TIdentifier)
    (Unit.assert-truthy (Scanner.Range? (Scanner.Token-location t-symbol-name?)))
    (Unit.assert-equals (Scanner.Coordinate-offset (Scanner.Range-start (Scanner.Token-location t-symbol-name?))) 8)
    (Unit.assert-equals (Scanner.Coordinate-offset (Scanner.Range-end (Scanner.Token-location t-symbol-name?))) 19)
    (Unit.assert-equals (Scanner.Token-lexeme t-symbol-name?) "symbol-name?")

    (Unit.assert-equals (Scanner.Token-token t-eos) Scanner.TEOS)
)