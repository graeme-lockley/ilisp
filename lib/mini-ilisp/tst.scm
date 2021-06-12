(import "../data/struct.scm" :names struct)
(import "../data/union.scm" :names union)
(import "../predicate.scm" :names boolean? integer? list-of? string?)
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

(struct BooleanLiteral
    (value boolean?)
)

(struct BinaryOperator
    (op binary-operator?)

    (op1 Expression?)
    (op2 Expression?)
)

(const (binary-operator? op)
    (or (= '+)
        (= '-)
        (= '*)
        (= '/)
        (= '=)
    )
)

(union Expression
    IdentifierReference? 
    BooleanLiteral? IntegerLiteral? StringLiteral?
    BinaryOperator?
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
