(import "../list.scm" :as List)
(import "../number.scm" :as Number)
(import "../string.scm" :as String)

(import "./ast.scm" :as AST)
(import "./environment.scm" :as Env)
(import "./tst.scm" :as TST)

; Translate the AST into a typed syntax tree.  The purpose of this 
; transformation is:
;
; - Ensure that all referenced identifiers refer to a known declaration.
; - Change the S-Expression representation into a representation that is more
;   amenable to translation into LLVM IR.
(const (ast->tst ast)
    (const env (Env.empty))

    (for-each '(
        "=" "<"
        "*" "-" "*" "/"
        "pair" "car" "cdr"
        "null?" "proc?" "integer?" "boolean?"
        "print" "println"
    ) (proc (n) 
        (Env.define-binding! env n)
    ))

    (List.map ast (proc (e) (expression->tst env e)))
)

(const- (expression->tst env e)
    (if (AST.IdentifierExpression? e)
            (if (Env.binding? env (AST.IdentifierExpression-id e))
                    (TST.IdentifierReference (AST.IdentifierExpression-id e) (AST.IdentifierExpression-location e))
                (raise 'UnknownIdentifier {:identifier (AST.IdentifierExpression-id e) :location (AST.IdentifierExpression-location e) })
            )
        (AST.LiteralIntExpression? e)
            (integer-literal-expression->tst env e)
        (AST.LiteralStringExpression? e)
            (string-literal-expression->tst env e)
        (AST.LiteralBooleanExpression? e)
            (TST.BooleanLiteral (AST.LiteralBooleanExpression-value e))
        (AST.S-Expression? e)
            (do (const expressions (AST.S-Expression-expressions e))
                (const first-expression (car expressions))

                (if (AST.IdentifierExpression? first-expression)
                    (do (const first-expression-identifier (AST.IdentifierExpression-id first-expression))
                        (if (= first-expression-identifier "println") (println-expression->tst env e)
                            (= first-expression-identifier "print") (print-expression->tst env e)
                            (= first-expression-identifier "+") (plus->tst env e)
                            (= first-expression-identifier "-") (minus->tst env e)
                            (= first-expression-identifier "*") (multiply->tst env e)
                            (= first-expression-identifier "/") (divide->tst env e)
                            (= first-expression-identifier "=") (equals->tst env e)
                            (= first-expression-identifier "<") (less-than->tst env e)

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

;; ; A const binding has 2 forms - single name and list of names.
;; (const- (const-expression->tst env e)
;;     (const es (AST.S-Expression-expressions e))
;;
;;     (if (< (count es) 3) (raise 'InvalidForm {:form 'const :syntax es :location (const expressions (AST.S-Expression-location e))}))
;;
;;     (const args (cadr es))
;;     (const body (cddr es))
;;
;;     ;; grab name from args
;;     ;; - error if name is already declared
;;     ;; - place declaration name into current scope
;;     ;; - open scope
;;     ;; - place parameter bindings into new scope
;;     ;;   - report error if duplicate parameter binding
;;     ;; - translate body within the context of the new scope
;;     ;; - close scope
;; )

(const- (plus->tst env e)
    (arithmetic-op->tst env e (TST.IntegerLiteral 0) '+)
)

(const- (minus->tst env e)
    (arithmetic-op->tst env e (TST.IntegerLiteral 0) '-)
)

(const- (multiply->tst env e)
    (arithmetic-op->tst env e (TST.IntegerLiteral 1) '*)
)

(const- (divide->tst env e)
    (arithmetic-op->tst env e (TST.IntegerLiteral 1) '/)
)

(const- (arithmetic-op->tst env e unary op)
    (const es (AST.S-Expression-expressions e))
    (const es' (List.map (cdr es) (proc (e') (expression->tst env e'))))

    (if (null? es') unary
        (= (count es') 1) (TST.BinaryOperator op unary (car es'))
        (fold (cddr es') (TST.BinaryOperator op (nth es' 0) (nth es' 1)) (proc (l r) (TST.BinaryOperator op l r)))
    )
)

(const- (equals->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (if (= (count es') 2) (TST.BinaryOperator '= (nth es' 0) (nth es' 1))
        (raise 'ArgumentsMismatch {:procedure '= :expected 2 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
    )
)

(const- (less-than->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (if (= (count es') 2) (TST.BinaryOperator '< (nth es' 0) (nth es' 1))
        (raise 'ArgumentsMismatch {:procedure '< :expected 2 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
    )
)

(const- (print-expression->tst env e)
    (const es (AST.S-Expression-expressions e))
    (const es' (List.map (cdr es) (proc (e') (expression->tst env e'))))

    (TST.CallPrint es')
)

(const- (println-expression->tst env e)
    (const es (AST.S-Expression-expressions e))
    (const es' (List.map (cdr es) (proc (e') (expression->tst env e'))))

    (TST.CallPrintLn es')
)

(const- (integer-literal-expression->tst env e)
    (const literal (AST.LiteralIntExpression-value e))
    (const literal-value (Number.string->number literal))
    
    (TST.IntegerLiteral literal-value)
)

(const- (string-literal-expression->tst env e)
    (const literal (AST.LiteralStringExpression-value e))
    (const literal-value (String.slice literal 1 (- (count literal) 1)))
    
    (TST.StringLiteral literal-value)
)
