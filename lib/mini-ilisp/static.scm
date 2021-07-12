(import "../data/struct.scm" :names struct)
(import "../list.scm" :as List)
(import "../number.scm" :as Number)
(import "../predicate.scm" :names integer? list-of? string?)
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

    ;; (for-each '(
    ;;     "=" "<"
    ;;     "*" "-" "*" "/"
    ;;     "pair" "car" "cdr"
    ;;     "null?" "boolean?" "integer?" "string?" "pair?" "proc?" 
    ;;     "print" "println"
    ;; ) (proc (n) 
    ;;     (Env.define-binding! env n)
    ;; ))

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

                (if (null? expressions) (TST.NullLiteral)
                    (do (const first-expression (car expressions))

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
                                    (= first-expression-identifier "if") (if->tst env e)
                                    (= first-expression-identifier "pair") (pair->tst env e)
                                    (= first-expression-identifier "car") (car->tst env e)
                                    (= first-expression-identifier "cdr") (cdr->tst env e)
                                    (= first-expression-identifier "null?") (null?->tst env e)
                                    (= first-expression-identifier "boolean?") (boolean?->tst env e)
                                    (= first-expression-identifier "integer?") (integer?->tst env e)
                                    (= first-expression-identifier "string?") (string?->tst env e)
                                    (= first-expression-identifier "pair?") (pair?->tst env e)
                                    (= first-expression-identifier "exit") (exit->tst env e)
                                    (= first-expression-identifier "const") (const-expression->tst env e)

                                    (do (const binding (Env.get env first-expression-identifier))

                                        (if (or (null? binding))
                                                (raise 'UnknownIdentifier {:identifier first-expression-identifier :location (AST.IdentifierExpression-location first-expression)})
                                            (not (Function? binding))
                                                (raise 'IdentifierNoFunction {:identifier first-expression-identifier :location (AST.IdentifierExpression-location first-expression)})
                                            (not (= (count expressions) (+ 1 (Function-number-of-parameters binding))))
                                                (raise 'ArgumentsMismatch {:procedure 'pair? :expected 1 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
                                            (TST.CallProcedure first-expression-identifier (List.map (cdr expressions) (proc (e) (expression->tst env e))))
                                        )
                                    )
                                )
                            )
                            (raise 'TODO-2 e)
                        )
                    )
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

(const- (const-expression->tst env e)
    ; form 1 (const x expr)
    ; form 2 (const (x1 ... x_n) e1 ... e_m)   n >= 1, m >= 1

    (const es (AST.S-Expression-expressions e))

    (if (< (count es) 3)
            (raise 'InvalidForm {:form 'const :syntax es :location (AST.S-Expression-location e)})
    )

    (const args (nth es 1))
    (const body (cddr es))

    (if (AST.IdentifierExpression? args)
            (if (> (count body) 1)
                    (raise 'InvalidForm {:form 'const :syntax es :location (AST.S-Expression-location e)})
                (do (const name args)
                    (const expr (car body))
                    (const expr' (expression->tst env expr))
                    (Env.define-binding! env (AST.IdentifierExpression-id name) (Variable))
                    (TST.ValueDeclaration (AST.IdentifierExpression-id name) expr')
                )
            )
        (and (AST.S-Expression? args) 
             ((list-of? AST.IdentifierExpression?) (AST.S-Expression-expressions args)) 
             (> (count (AST.S-Expression-expressions args)) 0))
            (do (const args' (List.map (AST.S-Expression-expressions args) AST.IdentifierExpression-id))
                (const name (car args'))
                (const arg-names (cdr args'))

                (Env.define-binding! env name (Function (count arg-names)))

                (const env' (Env.open-scope env))
                (for-each arg-names
                    (proc (arg-name)
                        (Env.define-binding! env' arg-name (Parameter))
                    )
                )

                (const body' (expressions->tst env' body))

                (TST.ProcedureDeclaration name arg-names body')                
            )
        (raise 'InvalidForm {:form 'const :syntax es :location (AST.S-Expression-location e)})        
    )
)

(const- (expressions->tst env es)
    (for-each es
        (proc (e)
            (if (AST.S-Expression? e)
                    (do (const es (AST.S-Expression-expressions e))
                        (if (and (= (count es) 3) 
                                 (AST.IdentifierExpression? (nth es 0)) 
                                 (= (AST.IdentifierExpression-id (nth es 0)) "const") 
                                 (AST.IdentifierExpression? (nth es 1)))
                                (Env.define-binding! env (AST.IdentifierExpression-id (nth es 1)) (Variable))
                        )
                    )
            )
        )
    )

    (List.map es (proc (e') (expression->tst env e')))
)

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

(const- (if->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (const (if-expressions->tst es)
        (if (null? es) (TST.NullLiteral)
            (do (const es-rest (cdr es))
                (if (null? es-rest)
                        (car es)
                    (TST.IfThenElse (car es) (car es-rest) (if-expressions->tst (cdr es-rest)))
                )
            )
        )
    )

    (if-expressions->tst es')
)

(const- (pair->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (if (= (count es') 2)
            (TST.Pair (nth es' 0) (nth es' 1))
        (raise 'ArgumentsMismatch {:procedure 'pair :expected 2 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
    )
)

(const- (car->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (if (= (count es') 1)
            (TST.Car (nth es' 0))
        (raise 'ArgumentsMismatch {:procedure 'car :expected 1 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
    )
)

(const- (cdr->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (if (= (count es') 1)
            (TST.Cdr (nth es' 0))
        (raise 'ArgumentsMismatch {:procedure 'cdr :expected 1 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
    )
)

(const- (null?->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (if (= (count es') 1)
            (TST.NullP (nth es' 0))
        (raise 'ArgumentsMismatch {:procedure 'null? :expected 1 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
    )
)

(const- (boolean?->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (if (= (count es') 1)
            (TST.BooleanP (nth es' 0))
        (raise 'ArgumentsMismatch {:procedure 'boolean? :expected 1 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
    )
)

(const- (integer?->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (if (= (count es') 1)
            (TST.IntegerP (nth es' 0))
        (raise 'ArgumentsMismatch {:procedure 'integer? :expected 1 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
    )
)

(const- (string?->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (if (= (count es') 1)
            (TST.StringP (nth es' 0))
        (raise 'ArgumentsMismatch {:procedure 'string? :expected 1 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
    )
)

(const- (pair?->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (if (= (count es') 1)
            (TST.PairP (nth es' 0))
        (raise 'ArgumentsMismatch {:procedure 'pair? :expected 1 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
    )
)

(const- (exit->tst env e)
    (const es (cdr (AST.S-Expression-expressions e)))
    (const es' (List.map es (proc (e') (expression->tst env e'))))

    (if (= (count es') 1)
            (TST.Exit (nth es' 0))
        (raise 'ArgumentsMismatch {:procedure 'exit :expected 1 :actual (count es') :location (AST.S-Expression-location e) :arguments es})
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

(struct Variable)
(struct Parameter)

(struct Function
    (number-of-parameters integer?)
)
