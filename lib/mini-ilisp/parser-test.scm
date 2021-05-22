(import "./parser.scm" :names IdentifierExpression? IdentifierExpression-id LiteralIntExpression? LiteralIntExpression-value scanner->ast)
(import "./scanner.scm" :names string->scanner)

(import "../unit.scm" :as Unit)

(const (parse text)
    (scanner->ast (string->scanner text))
)

(Unit.test "Identifier"
    (const ast (parse "hello"))

    (Unit.assert-equals (count ast) 1)

    (const ast' (car ast))
    (Unit.assert-truthy (IdentifierExpression? ast'))
    (Unit.assert-equals (IdentifierExpression-id ast') "hello")
)

(Unit.test "Literal Int"
    (const ast (parse "-123"))

    (Unit.assert-equals (count ast) 1)

    (const ast' (car ast))
    (Unit.assert-truthy (LiteralIntExpression? ast'))
    (Unit.assert-equals (LiteralIntExpression-value ast') "-123")
)
