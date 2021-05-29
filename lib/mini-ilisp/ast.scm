(import "../data/struct.scm" :names struct)
(import "../data/union.scm" :names union)
(import "../predicate.scm" :names boolean? list-of? string?)

(struct IdentifierExpression
    (id string?)
    (location Scanner.Location?)
)

(struct LiteralBooleanExpression
    (value boolean?)
    (location Scanner.Location?)
)

(struct LiteralIntExpression
    (value string?)
    (location Scanner.Location?)
)

(struct LiteralStringExpression
    (value string?)
    (location Scanner.Location?)
)

(struct S-Expression
    (expressions (list-of? Expression?))
    (location Scanner.Location?)
)

(union Expression
    IdentifierExpression? LiteralIntExpression? LiteralStringExpression? LiteralBooleanExpression? S-Expression?
)
