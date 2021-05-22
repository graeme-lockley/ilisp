(import "../unit.scm" :as Unit)

(import "./scanner.scm" :as Scanner)

(Unit.test "Coordinate"
    (const c (Scanner.Coordinate 1 2 3))

    (Unit.assert-truthy (Scanner.Coordinate? c))
    (Unit.assert-equals (Scanner.Coordinate-offset c) 1)
    (Unit.assert-equals (Scanner.Coordinate-line c) 2)
    (Unit.assert-equals (Scanner.Coordinate-column c) 3)
)

(const- (string->tokens s)
    (const (tokens scanner)
        (const token (Scanner.Scanner-current-token scanner))

        ;; (println "tokens: " (pr-str token))

        (if (= (Scanner.Token-token token) Scanner.TEOS) (list token)
            (do (Scanner.next scanner)
                (pair token (tokens scanner))
            )
        )
    )

    (tokens (Scanner.string->scanner s))
)

(const- (assert-coordinate-token token token-type offset lexeme)
    (Unit.assert-equals (Scanner.Token-token token) token-type)
    (Unit.assert-truthy (Scanner.Coordinate? (Scanner.Token-location token)))
    (Unit.assert-equals (Scanner.Coordinate-offset (Scanner.Token-location token)) offset)
    (Unit.assert-equals (Scanner.Token-lexeme token) lexeme)
)

(const- (assert-range-token token token-type offset-start offset-end lexeme)
    (Unit.assert-equals (Scanner.Token-token token) token-type)
    (Unit.assert-truthy (Scanner.Range? (Scanner.Token-location token)))
    (Unit.assert-equals (Scanner.Coordinate-offset (Scanner.Range-start (Scanner.Token-location token))) offset-start)
    (Unit.assert-equals (Scanner.Coordinate-offset (Scanner.Range-end (Scanner.Token-location token))) offset-end)
    (Unit.assert-equals (Scanner.Token-lexeme token) lexeme)
)

(Unit.test "initialise scanner"
    (const (verify text offset line column)
        (const scanner (Scanner.string->scanner text))

        (Unit.assert-equals (Scanner.Scanner-current-token scanner) (Scanner.Token Scanner.TEOS (Scanner.Coordinate offset line column) ()))
    )

    (verify "" 0 1 1)
    (verify " " 1 1 2)
    (verify "  " 2 1 3)
)

(Unit.test "match identifier"
    (const ts (string->tokens "a hello symbol-name?"))

    (Unit.assert-equals (count ts) 4)

    (assert-coordinate-token (nth ts 0) Scanner.TIdentifier 0 "a")
    (assert-range-token (nth ts 1) Scanner.TIdentifier 2 6 "hello")
    (assert-range-token (nth ts 2) Scanner.TIdentifier 8 19 "symbol-name?")
    (assert-coordinate-token (nth ts 3) Scanner.TEOS 20 ())
)

(Unit.test "match literal int"
    (const ts (string->tokens "0 10 123456789 -0 -10 -123456789"))

    (Unit.assert-equals (count ts) 7)
    (assert-coordinate-token (nth ts 0) Scanner.TLiteralInt 0 "0")
    (assert-range-token (nth ts 1) Scanner.TLiteralInt 2 3 "10")
    (assert-range-token (nth ts 2) Scanner.TLiteralInt 5 13 "123456789")
    (assert-range-token (nth ts 3) Scanner.TLiteralInt 15 16 "-0")
    (assert-range-token (nth ts 4) Scanner.TLiteralInt 18 20 "-10")
    (assert-range-token (nth ts 5) Scanner.TLiteralInt 22 31 "-123456789")
    (assert-coordinate-token (nth ts 6) Scanner.TEOS 32 ())
)

(Unit.test "match literal string"
    (const ts (string->tokens "\"\" \"hello world\" \"a\\\"b\""))

    (Unit.assert-equals (count ts) 4)

    (assert-range-token (nth ts 0) Scanner.TLiteralString 0 1 "\"\"")
    (assert-range-token (nth ts 1) Scanner.TLiteralString 3 15 "\"hello world\"")
    (assert-range-token (nth ts 2) Scanner.TLiteralString 17 22 "\"a\\\"b\"")
    (assert-coordinate-token (nth ts 3) Scanner.TEOS 23 ())
)

(Unit.test "match literal boolean"
    (const ts (string->tokens "#t #f"))

    (Unit.assert-equals (count ts) 3)

    (assert-range-token (nth ts 0) Scanner.TLiteralTrue 0 1 ())
    (assert-range-token (nth ts 1) Scanner.TLiteralFalse 3 4 ())
    (assert-coordinate-token (nth ts 2) Scanner.TEOS 5 ())
)

(Unit.test "match parenthesis"
    (const ts (string->tokens "( ) ()"))

    ;; (println (pr-str ts))

    (Unit.assert-equals (count ts) 5)

    (assert-coordinate-token (nth ts 0) Scanner.TLParen 0 ())
    (assert-coordinate-token (nth ts 1) Scanner.TRParen 2 ())
    (assert-coordinate-token (nth ts 2) Scanner.TLParen 4 ())
    (assert-coordinate-token (nth ts 3) Scanner.TRParen 5 ())
    (assert-coordinate-token (nth ts 4) Scanner.TEOS 6 ())
)
