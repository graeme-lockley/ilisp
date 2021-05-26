(import "../data/struct.scm" :names struct)
(import "../data/union.scm" :names union)
(import "../list.scm" :as List)
(import "./scanner.scm" :as Scanner)
(import "./parser.scm" :as Parser)
(import "../predicate.scm" :names string?)

; Translate the AST into a typed syntax tree.  The purpose of this 
; transformation is:
;
; - Ensure that all referenced identifiers refer to a known declaration.
; - Change the S-Expression representation into a representation that is more
;   amenable to translation into LLVM IR.
(const (ast->tst ast)
    (const names (list->mutable-set '(
        "=" "<"
        "*" "-" "*" "/"
        "cons" "car" "cdr"
        "null?" "proc?" "integer?" "boolean?"
        "print" "println"
    )))

    (const (expression->tst e)
        (if (Parser.IdentifierExpression? e)
                (if (contains? names (Parser.IdentifierExpression-id e))
                        (IdentifierReference (Parser.IdentifierExpression-id e) (Parser.IdentifierExpression-location e))
                    (raise 'UnknownIdentifier {:identifier (Parser.IdentifierExpression-id e) :location (Parser.IdentifierExpression-location e) })
                )
            (raise 'TODO e)
        )
    )

    (List.map ast expression->tst)
)

(const (list->mutable-set lst)
    (const (map-args vs)
        (if (null? vs) ()
            (pair (car vs) (pair (car vs) (map-args (cdr vs))))
        )
    )

    (*builtin*.apply *builtin*.mutable-map (map-args lst))
)

(const (contains? s e)
    (*builtin*.map-contains? s e)
)

(struct DeclareValue
    (name string?)
    (expr Expression?)
)

(struct IdentifierReference
    (name string?)
    (location Scanner.Location?)
)

(union Expression
    string?
)
