(import "../data/struct.scm" :names struct)
(import "../data/union.scm" :names union)
(import "../predicate.scm" :names =? boolean? integer? list-of? or? string?)
(import "./scanner.scm" :as Scanner)

(struct DeclareValue
    (name string?)
    (expr Expression?)
)

(struct IdentifierReference
    (name string?)
    (location Scanner.Location?)
)

(struct NullLiteral
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

(const binary-operator?
    (or? (=? '+)
         (=? '-)
         (=? '*)
         (=? '/)
         (=? '=)
         (=? '<)
    )
)

(struct IfThenElse
    (e1 Expression?)
    (e2 Expression?)
    (e3 Expression?)
)

(struct Pair
    (car Expression?)
    (cdr Expression?)
)

(struct Car
    (e Expression?)
)

(struct Cdr
    (e Expression?)
)

(struct NullP
    (e Expression?)
)

(struct BooleanP
    (e Expression?)
)

(struct IntegerP
    (e Expression?)
)

(struct StringP
    (e Expression?)
)

(struct PairP
    (e Expression?)
)

(union Expression
    IdentifierReference? 
    NullLiteral? BooleanLiteral? IntegerLiteral? StringLiteral?
    BinaryOperator?
    IfThenElse?
    Pair? Car? Cdr?
    NullP? BooleanP? IntegerP? StringP? PairP?
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
