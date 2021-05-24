(import "./parser.scm" :names 
    IdentifierExpression? IdentifierExpression-id 
    LiteralBooleanExpression? LiteralBooleanExpression-value 
    LiteralIntExpression? LiteralIntExpression-value 
    LiteralStringExpression? LiteralStringExpression-value 
    S-Expression? S-Expression-expressions
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

(Unit.test "Abbreviated S-Expression"
    (const ast (parse "(hello 1 \"a\" #t (world))"))

    (Unit.assert-equals (count ast) 1)

    (const ast' (car ast))
    (Unit.assert-truthy (S-Expression? ast'))

    (const hello' (nth (S-Expression-expressions ast') 0))
    (Unit.assert-truthy (IdentifierExpression? hello'))
    (Unit.assert-equals (IdentifierExpression-id hello') "hello")

    (const int' (nth (S-Expression-expressions ast') 1))
    (Unit.assert-truthy (LiteralIntExpression? int'))
    (Unit.assert-equals (LiteralIntExpression-value int') "1")

    (const string' (nth (S-Expression-expressions ast') 2))
    (Unit.assert-truthy (LiteralStringExpression? string'))
    (Unit.assert-equals (LiteralStringExpression-value string') "\"a\"")

    (const boolean' (nth (S-Expression-expressions ast') 3))
    (Unit.assert-truthy (LiteralBooleanExpression? boolean'))
    (Unit.assert-truthy (LiteralBooleanExpression-value boolean'))

    (const ast'' (nth (S-Expression-expressions ast') 4))
    (Unit.assert-truthy (S-Expression? ast''))
)
