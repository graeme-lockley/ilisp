(import "../data/struct.scm" :names struct)
(import "../data/union.scm" :names union)
(import "../predicate.scm" :names integer? list-of? string?)
(import "./scanner.scm" :as Scanner)

(struct DeclareValue
    (name string?)
    (expr Expression?)
)

(struct IdentifierReference
    (name string?)
    (location Scanner.Location?)
)

(struct IntegerLiteral
    (value integer?)
)

(struct StringLiteral
    (value string?)
)

(union Expression
    IdentifierReference? IntegerLiteral? StringLiteral?
)

(struct CallPrintLn
    (args (list-of? Expression?))
)

(struct CallPrint
    (args (list-of? Expression?))
)

(union Statement
    CallPrint? CallPrintLn?
)
