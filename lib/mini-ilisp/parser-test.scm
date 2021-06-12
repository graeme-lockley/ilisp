(import "./ast.scm" :names
    IdentifierExpression? IdentifierExpression-id 
    LiteralBooleanExpression? LiteralBooleanExpression-value 
    LiteralIntExpression? LiteralIntExpression-value 
    LiteralStringExpression? LiteralStringExpression-value 
    S-Expression? S-Expression-expressions
)
(import "./parser.scm" :names scanner->ast)
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

(Unit.test "Literal ()"
    (const ast (parse "()"))

    (Unit.assert-equals (count ast) 1)

    (const ast-t' (nth ast 0))
    (Unit.assert-truthy (S-Expression? ast-t'))
    (Unit.assert-equals (count (S-Expression-expressions ast-t')) 0)
)

(Unit.test "Abbreviated S-Expression"
    (const ast (parse "(hello 1 \"a\" #t (world))"))

    (Unit.assert-equals (count ast) 1)

    (Unit.assert-truthy (S-Expression? (car ast)))
    (const ast' (S-Expression-expressions (car ast)))
    (Unit.assert-equals (count ast') 5)

    (const hello' (nth ast' 0))
    (Unit.assert-truthy (IdentifierExpression? hello'))
    (Unit.assert-equals (IdentifierExpression-id hello') "hello")

    (const int' (nth ast' 1))
    (Unit.assert-truthy (LiteralIntExpression? int'))
    (Unit.assert-equals (LiteralIntExpression-value int') "1")

    (const string' (nth ast' 2))
    (Unit.assert-truthy (LiteralStringExpression? string'))
    (Unit.assert-equals (LiteralStringExpression-value string') "\"a\"")

    (const boolean' (nth ast' 3))
    (Unit.assert-truthy (LiteralBooleanExpression? boolean'))
    (Unit.assert-truthy (LiteralBooleanExpression-value boolean'))

    (Unit.assert-truthy (S-Expression?  (nth ast' 4)))
    (const ast'' (S-Expression-expressions (nth ast' 4)))
    (Unit.assert-equals (count ast'') 1)

    (const world' (nth ast'' 0))
    (Unit.assert-truthy (IdentifierExpression? world'))
    (Unit.assert-equals (IdentifierExpression-id world') "world")
)
