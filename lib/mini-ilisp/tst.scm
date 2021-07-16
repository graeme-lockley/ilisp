(import "../data/struct.scm" :names struct)
(import "../data/union.scm" :names union)
(import "../predicate.scm" :names =? boolean? integer? list-of? or? string?)
(import "./scanner.scm" :as Scanner)

(struct IdentifierReference
    (name string?)
    (location Scanner.Location?)
)

(struct CallProcedure
    (name string?)
    (arguments (list-of? Expression?))
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

(struct Exit
    (e Expression?)
)

(struct AssertEq
    (msg Expression?)
    (v1 Expression?)
    (v2 Expression?)
)

(struct AssertNeq
    (msg Expression?)
    (v1 Expression?)
    (v2 Expression?)
)

(struct AssertTrue
    (msg Expression?)
    (v Expression?)
)

(struct AssertFalse
    (msg Expression?)
    (v Expression?)
)

(struct Fail
    (msg Expression?)
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

(struct ValueDeclaration
    (name string?)
    (e Expression?)
)

(struct ProcedureDeclaration
    (name string?)
    (arg-names (list-of? string?))
    (es (list-of? Expression?))
)

(struct CallPrintLn
    (args (list-of? Expression?))
)

(struct CallPrint
    (args (list-of? Expression?))
)

(struct Do
    (es (list-of? Statement?))
)

(union Expression
    IdentifierReference? CallProcedure?
    NullLiteral? BooleanLiteral? IntegerLiteral? StringLiteral?
    BinaryOperator?
    IfThenElse? Do?
    Pair? Car? Cdr? Exit?
    AssertEq? AssertNeq? AssertTrue? AssertFalse? Fail?
    NullP? BooleanP? IntegerP? StringP? PairP?
    CallPrintLn? CallPrint? Do?
    ValueDeclaration? ProcedureDeclaration?
)

(union Statement
    CallPrint? CallPrintLn? Do? Expression?
)
