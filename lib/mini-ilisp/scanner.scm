(import "../character.scm" :names ->character)
(import "../data/struct.scm" :names mutable-struct struct)
(import "../data/union.scm" :names union)
(import "../number.scm" :names dec inc)
(import "../predicate.scm" :names =? byte-vector? character? integer? null? string?)

; Given a byte vector will return a scanner.  This scanner has a number of
; procedures that can be applied to it to determine whether the end of input
; has been reached, what the current token is and then be able to step over
; this current token.
;
; The following is the lexical definition of `mini-iLisp`.  The notation used is
; [Scanpiler](https://github.com/littlelanguages/scanpiler).
;
; ```
; tokens
;     Identifier = (id \ '-') {digit | id};
;     LiteralInt = ['-'] digit {digit};
;     LiteralString = '"' {!('"' + cr) | "\" ('"' + '\'}  '"';
;     LiteralTrue = "#t";
;     LiteralFalse = "#f";
;     LParen = "(";
;     RParen = ")";
;
; comments
;    ";" {!cr};
;
; whitespace
;   chr(0)-' ';
;
; fragments
;   digit = '0'-'9';
;   id = '!'-'}' \ ('0'-'9' + '"' + '(' + ')' + ';' + '#');
;   cr = chr(10);
; ```
;
; :usage (mk-scanner input)
; :parameter input byte-vector?
;   The input to be scanned.
; :returns Scanner?
(const (byte-vector->scanner input)
    (const scanner 
        (Scanner 
            input 
            (count input)
            (- 2)  ; offste
            1      ; line
            (- 1)  ; column
            #x1
            0
            0
            0
            (Token TERROR (Coordinate 0 0 0) "")
        )
    )

    (next scanner)
)

(const string->scanner
    (import "../byte-vector.scm" :names string->byte-vector)

    (|> string->byte-vector byte-vector->scanner)
)

(const (next-token scanner)
    (const token (Scanner-current-token scanner))
    (next scanner)
    token
)

(const (next scanner)
    (const (skip-white-space)
        (if (<= #x1 (Scanner-next-ch scanner) #x20)
                (do (next-character)
                    (skip-white-space)
                )
            ()
        )
    )

    (const (mark-start)
        (Scanner-start-offset! scanner (Scanner-offset scanner))
        (Scanner-start-line! scanner (Scanner-line scanner))
        (Scanner-start-column! scanner (Scanner-column scanner))
    )

    (const (next-character)
        (Scanner-offset! scanner (inc (Scanner-offset scanner)))

        (if (= (Scanner-next-ch scanner) #xa)
                (do (Scanner-column! scanner 0)
                    (Scanner-line! scanner (inc (Scanner-line scanner)))
                )
            (Scanner-column! scanner (inc (Scanner-column scanner)))
        )

        (const next-offset (inc (Scanner-offset scanner)))

        (if (< next-offset (Scanner-input-length scanner))
                (Scanner-next-ch! scanner (->character (nth (Scanner-input scanner) next-offset)))
            (Scanner-next-ch! scanner #x0)
        )
    )

    (const (set-token t . lexeme)
        (const loc
            (if (= (Scanner-start-offset scanner) (Scanner-offset scanner)) 
                    (Coordinate (Scanner-start-offset scanner) (Scanner-start-line scanner) (Scanner-start-column scanner))
                (Range
                    (Coordinate (Scanner-start-offset scanner) (Scanner-start-line scanner) (Scanner-start-column scanner))
                    (Coordinate (Scanner-offset scanner) (Scanner-line scanner) (Scanner-column scanner))
                )
            )
        )

        (const current-token (Token
            t
            loc
            (if (null? lexeme) (slice-lexeme) (car lexeme))
        ))

        (Scanner-current-token! scanner current-token)
    )

    (const (slice-lexeme)
        (import "../byte-vector.scm" :names slice->string)
        (slice->string (Scanner-input scanner) (Scanner-start-offset scanner) (inc (Scanner-offset scanner)))
    )

    (const (while-next p)
        (const ch (Scanner-next-ch scanner))

        (if (p ch)
                (do (next-character)
                    (while-next p)
                )
            ()
        )
    )

    (const (scan-literal-string)
        (const (scan-next)
            (const ch (Scanner-next-ch scanner))

            (if (< ch #x20)
                    (set-token TERROR)
                (= ch #x5c)
                    (do (next-character)
                        (if (or (= ch #x22) (= ch #x5c))
                                (do (next-character)
                                    (scan-next)
                                )
                            (do (next-character)
                                (set-token TERROR)
                            )
                        )
                    )
                (= ch #x22)
                    (do (next-character)
                        (set-token TLiteralString)
                    )
                (do (next-character)
                    (scan-next)
                )
            )
        )

        (scan-next)
    )

    (if (= (current-token scanner) TEOS) scanner
        (do (skip-white-space)
            (const ch (Scanner-next-ch scanner))

            (next-character)
            (mark-start)

            (if (= ch #x0) 
                    (set-token TEOS ())
                (= ch #\-)
                    (if (digit? (Scanner-next-ch scanner))
                            (do (while-next digit?)
                                (set-token TLiteralInt)
                            )
                        (set-token TIdentifier)
                    )
                (digit? ch)
                    (do (while-next digit?)
                        (set-token TLiteralInt)
                    )
                (id? ch)
                    (do (while-next (or? id? digit?))
                        (set-token TIdentifier)
                    )
                (= ch #x22)
                    (scan-literal-string)
                (= ch #x23)
                    (do (const ch' (Scanner-next-ch scanner))

                        (if (= ch' #\t) 
                                (do (next-character)
                                    (set-token TLiteralTrue ())
                                )
                            (= ch' #\f)
                                (do (next-character)
                                    (set-token TLiteralFalse ())
                                )
                            (set-token TERROR)
                        )
                    )
                (= ch #x28)
                    (set-token TLParen ())
                (= ch #x29)
                    (set-token TRParen ())
                (set-token TERROR)
            )

            scanner
        )
    )
)

; id = '!'-'}' \ ('0'-'9' + '"' + '(' + ')' + ';');
(const (id? c)
    (and (<= #x21 c #x7e)
         (not (or (<= #\0 c #\9)
                  (= #x22 c) ; '"'
                  (= #x23 c) ; '#'
                  (= #x28 c) ; '('
                  (= #x29 c) ; ')'
                  (= #x3b c) ; ';'
              )
         )
    )
)

; digit = '0'-'9';
(const (digit? c)
    (<= #\0 c #\9)
)

(const- (current-token scanner)
    (Token-token (Scanner-current-token scanner))
)

(const TIdentifier 0)
(const TLiteralInt 1)
(const TLiteralString 2)
(const TLiteralTrue 3)
(const TLiteralFalse 4)
(const TLParen 5)
(const TRParen 6)
(const TEOS 7)
(const TERROR 8)

(struct Token
    (token (proc (n) (<= TIdentifier n TERROR)))
    (location Location?)
    (lexeme (or? string? null?))
)

(struct Coordinate
    (offset integer?)
    (line integer?)
    (column integer?)
)

(struct Range
    (start Coordinate?)
    (end Coordinate?)
)

(union Location
    Coordinate? Range?
)

(mutable-struct Scanner
    (input byte-vector?)
    (input-length integer?)
    (offset integer?)
    (line integer?)
    (column integer?)
    (next-ch character?)
    (start-offset integer?)
    (start-line integer?)
    (start-column integer?)
    (current-token Token?)
)

(const (combine loc1 loc2)
    (const (min-coordinate c1 c2)
        (if (< (Coordinate-offset c1) (Coordinate-offset c2))
                c1
            c2
        )
    )

    (const (max-coordinate c1 c2)
        (if (> (Coordinate-offset c1) (Coordinate-offset c2))
                c1
            c2
        )
    )

    (if (and (Coordinate? loc1) (Coordinate? loc2))
            (if (= (Coordinate-offset loc1) (Coordinate-offset loc2))
                    loc1
                (< (Coordinate-offset loc1) (Coordinate-offset loc2))
                    (Range loc1 loc2)
                (Range loc2 loc1)
            )
        (Coordinate? loc1)
            (Range (min-coordinate loc1 (Range-start loc2)) (max-coordinate loc1 (Range-end loc2)))
        (Coordinate? loc2)
            (Range (min-coordinate (Range-start loc1) loc2) (max-coordinate (Range-end loc1) loc2))
        (Range (min-coordinate (Range-start loc1) (Range-start loc2)) (max-coordinate (Range-end loc1) (Range-end loc2)))
    )
)
