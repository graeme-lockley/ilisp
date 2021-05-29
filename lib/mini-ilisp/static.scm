(import "./ast.scm" :as AST)
(import "./environment.scm" :as Env)
(import "../data/struct.scm" :names struct)
(import "../data/union.scm" :names union)
(import "../list.scm" :as List)
(import "./scanner.scm" :as Scanner)
(import "../predicate.scm" :names list-of? string?)
(import "../string.scm" :as String)

; Translate the AST into a typed syntax tree.  The purpose of this 
; transformation is:
;
; - Ensure that all referenced identifiers refer to a known declaration.
; - Change the S-Expression representation into a representation that is more
;   amenable to translation into LLVM IR.
(const (ast->tst ast)
    (const (println-expression->tst env e)
        (const es (AST.S-Expression-expressions e))
        (const es' (List.map (cdr es) (proc (e') (expression->tst env e'))))

        (CallPrintLn es')
    )

    ;; ; A const binding has 2 forms - single name and list of names.
    ;; (const (const-expression->tst env e)
    ;;     (const es (AST.S-Expression-expressions e))

    ;;     (if (< (count es) 3) (raise 'InvalidForm {:form 'const :syntax es :location (const expressions (AST.S-Expression-location e))}))

    ;;     (const args (cadr es))
    ;;     (const body (cddr es))

    ;;     ;; grab name from args
    ;;     ;; - error if name is already declared
    ;;     ;; - place declaration name into current scope
    ;;     ;; - open scope
    ;;     ;; - place parameter bindings into new scope
    ;;     ;;   - report error if duplicate parameter binding
    ;;     ;; - translate body within the context of the new scope
    ;;     ;; - close scope
    ;; )

    (const (expression->tst env e)
        (if (AST.IdentifierExpression? e)
                (if (Env.binding? env (AST.IdentifierExpression-id e))
                        (IdentifierReference (AST.IdentifierExpression-id e) (AST.IdentifierExpression-location e))
                    (raise 'UnknownIdentifier {:identifier (AST.IdentifierExpression-id e) :location (AST.IdentifierExpression-location e) })
                )
            (AST.LiteralStringExpression? e)
                (string-literal-expression->tst env e)
            (AST.S-Expression? e)
                (do (const expressions (AST.S-Expression-expressions e))
                    (const first-expression (car expressions))

                    (if (AST.IdentifierExpression? first-expression)
                        (do (const first-expression-identifier (AST.IdentifierExpression-id first-expression))
                            (if (= first-expression-identifier "println") (println-expression->tst env e)
                                ;; (= first-expression-identifier "const") (const-expression->tst env e)
                                (raise 'TODO-1 e)
                            )
                        )
                        (raise 'TODO-2 e)
                    )
                )
            (raise 'TODO-3 e)
        )
    )

    (const env (Env.empty))

    (for-each '(
        "=" "<"
        "*" "-" "*" "/"
        "cons" "car" "cdr"
        "null?" "proc?" "integer?" "boolean?"
        "print" "println"
    ) (proc (n) 
        (Env.define-binding! env n)
    ))

    (List.map ast (proc (e) (expression->tst env e)))
)

(const (string-literal-expression->tst env e)
    (const literal (AST.LiteralStringExpression-value e))
    (const literal-value (String.slice literal 1 (- (count literal) 1)))
    
    (StringLiteral literal-value)
)

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
