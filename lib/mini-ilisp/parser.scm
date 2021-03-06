(import "./ast.scm" :as AST)
(import "./scanner.scm" :as Scanner)

; A parser for mini-Lisp which accepts a scanner and returns an AST.
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
; :returns (list-of? Expression?)
; :signal SyntaxError
;   A syntax error occured whilst parsing.
(const (scanner->ast scanner)
    (const (current-token)
        (Scanner.Scanner-current-token scanner)
    )

    (const (current-token-type)
        (Scanner.Token-token (current-token))
    )

    (const (match-token-type token-type)
        (const token (current-token))

        (if (= (Scanner.Token-token token) token-type)
                (Scanner.next-token scanner)
            (raise 'SyntaxError {:found (Scanner.Token-token token) :expected (list token-type) :location (Scanner.Token-location token)})
        )
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
                    (AST.IdentifierExpression (Scanner.Token-lexeme token') (Scanner.Token-location token'))
                )
            (= token-type Scanner.TLiteralInt)
                (do (const token' (Scanner.next-token scanner))
                    (AST.LiteralIntExpression (Scanner.Token-lexeme token') (Scanner.Token-location token'))
                )
            (= token-type Scanner.TLiteralString)
                (do (const token' (Scanner.next-token scanner))
                    (AST.LiteralStringExpression (Scanner.Token-lexeme token') (Scanner.Token-location token'))
                )
            (= token-type Scanner.TLiteralTrue)
                (do (const token' (Scanner.next-token scanner))
                    (AST.LiteralBooleanExpression #t (Scanner.Token-location token'))
                )
            (= token-type Scanner.TLiteralFalse)
                (do (const token' (Scanner.next-token scanner))
                    (AST.LiteralBooleanExpression #f (Scanner.Token-location token'))
                )
            (= token-type Scanner.TLParen)
                (do (const lparen (Scanner.next-token scanner))
                    (const body (expression'))
                    (const rparen (match-token-type Scanner.TRParen))
                    (AST.S-Expression body (Scanner.combine (Scanner.Token-location lparen) (Scanner.Token-location rparen)))
                )
            (raise 'SyntaxError {:found token-type :expected (list Scanner.TIdentifier) :location (Scanner.Token-location token)})
        )
    )

    (const es (expression'))
    (match-token-type Scanner.TEOS)
    es
)
