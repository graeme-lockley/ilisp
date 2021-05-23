(import "./parser.scm" :names 
    IdentifierExpression? IdentifierExpression-id 
    LiteralBooleanExpression? LiteralBooleanExpression-value 
    LiteralIntExpression? LiteralIntExpression-value 
    LiteralStringExpression? LiteralStringExpression-value 
    scanner->ast)
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

(Unit.test "Literal String"
    (const ast (parse "\"hello\""))

    (Unit.assert-equals (count ast) 1)

    (const ast' (car ast))
    (Unit.assert-truthy (LiteralStringExpression? ast'))
    (Unit.assert-equals (LiteralStringExpression-value ast') "\"hello\"")
)

(Unit.test "Literal Boolean"
    (const ast (parse "#t #f"))

    (Unit.assert-equals (count ast) 2)

    (const ast-t' (nth ast 0))
    (Unit.assert-truthy (LiteralBooleanExpression? ast-t'))
    (Unit.assert-truthy (LiteralBooleanExpression-value ast-t'))

    (const ast-f' (nth ast 1))
    (Unit.assert-truthy (LiteralBooleanExpression? ast-f'))
    (Unit.assert-falsy (LiteralBooleanExpression-value ast-f'))
)
