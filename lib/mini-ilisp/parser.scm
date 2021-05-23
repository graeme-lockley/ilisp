(import "../data/struct.scm" :names struct)
(import "../data/union.scm" :names union)
(import "../predicate.scm" :names boolean? string?)
(import "./scanner.scm" :as Scanner)

; A parser for mini-Lisp which accepts a scanner and then returns an AST.
;
; The grammar for this parser, using [Parspiler](https://github.com/littlelanguages/parspiler) notation:
; ```
; Program: {Expression} EOS;
;
; Expression
;   : "(" {Expression} ")"
;   | Identifier
;   | LiteralInt
;   | LiteralString
;   | LiteralTrue
;   | LiteralFalse
;   ;
; ```
;
; :usage (scanner->ast scanner)
; :parameter scanner Scanner?
; :returns Ast?
; :signal SyntaxError
;   A syntax error occured whilst parsing.
(const (scanner->ast scanner)
    (const (current-token)
        (Scanner.Scanner-current-token scanner)
    )

    (const (current-token-type)
        (Scanner.Token-token (current-token))
    )

    (const (first-expression?)
        (const token-type (current-token-type))

        (or (= token-type Scanner.TLParen)
            (= token-type Scanner.TIdentifier)
            (= token-type Scanner.TLiteralInt)
            (= token-type Scanner.TLiteralString)
            (= token-type Scanner.TLiteralTrue)
            (= token-type Scanner.TLiteralFalse)
        )
    )

    (const (expression')
        (if (first-expression?)
                (pair (expression) (expression'))
            ()
        )
    )

    (const (expression)
        (const token (current-token))
        (const token-type (Scanner.Token-token token))

        (if (= token-type Scanner.TIdentifier)
                (do (const token' (Scanner.next-token scanner))
                    (IdentifierExpression (Scanner.Token-lexeme token') (Scanner.Token-location token'))
                )
            (= token-type Scanner.TLiteralInt)
                (do (const token' (Scanner.next-token scanner))
                    (LiteralIntExpression (Scanner.Token-lexeme token') (Scanner.Token-location token'))
                )
            (= token-type Scanner.TLiteralString)
                (do (const token' (Scanner.next-token scanner))
                    (LiteralStringExpression (Scanner.Token-lexeme token') (Scanner.Token-location token'))
                )
            (= token-type Scanner.TLiteralTrue)
                (do (const token' (Scanner.next-token scanner))
                    (LiteralBooleanExpression #t (Scanner.Token-location token'))
                )
            (= token-type Scanner.TLiteralFalse)
                (do (const token' (Scanner.next-token scanner))
                    (LiteralBooleanExpression #f (Scanner.Token-location token'))
                )
            (raise 'SyntaxError {:found token-type :expected (list Scanner.TIdentifier) :location (Scanner.Token-location token)})
        )
    )

    (const es (expression'))
    ;; (match Scanner.TEOS)
    es
)


;; (struct Program
;;     (es (list-of? Expression?))
;; )

(struct IdentifierExpression
    (id string?)
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

(struct LiteralBooleanExpression
    (value boolean?)
    (location Scanner.Location?)
)

(union Expression
    IdentifierExpression? LiteralIntExpression?
)