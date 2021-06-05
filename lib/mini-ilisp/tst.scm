(import "../data/struct.scm" :names struct)
(import "../data/union.scm" :names union)
(import "../predicate.scm" :names list-of? string?)
(import "./scanner.scm" :as Scanner)

(struct DeclareValue
    (name string?)
    (expr Expression?)
)

(struct IdentifierReference
    (name string?)
    (location Scanner.Location?)
)

(struct StringLiteral
    (value string?)
)

(union Expression
    IdentifierReference? StringLiteral?
)

(struct CallPrintLn
    (args (list-of? Expression?))
)

(union Statement
    CallPrintLn?
)
