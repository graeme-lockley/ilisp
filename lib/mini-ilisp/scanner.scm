(import "../character.scm" :names ->character)
(import "../data/struct.scm" :names mutable-struct struct)
(import "../data/union.scm" :names union)
(import "../number.scm" :names dec inc)
(import "../predicate.scm" :names byte-vector? character? integer? null? string?)

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
;     Identifier = id {digit | id};
;     LiteralInt = digits;
;     LiteralString = '"' {!('"' + cr) | "\" '"'}  '"';
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
;   digits = digit {digit};
;   id = '!'-'}' \ ('0'-'9' + '"' + '(' + ')' + ';');
;   cr = chr(10);
; ```
;
; :usage (mk-scanner input)
; :parameter input byte-vector?
;   The input to be scanned.
; :returns Scanner?
(const (mk-scanner input)
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

    (const (next-character-and-mark-start)
        (next-character)
        (mark-start)
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
        (slice->string (Scanner-input scanner) (Scanner-start-offset scanner) (Scanner-offset scanner))
    )

    (if (= (current-token scanner) TEOS) scanner
        (do (skip-white-space)
            (const ch (Scanner-next-ch scanner))

            (if (= ch #x0) 
                    (do (next-character-and-mark-start)
                        (set-token TEOS)
                    )
                (set-token TERROR "")
            )

            scanner
        )
    )
)

(const- (current-token scanner)
    (Token-token (Scanner-current-token scanner))
)

(const TIdentifier 0)
(const TLiteralInt 1)
(const TListernString 2)
(const TLiteralTrue 3)
(const TLiteralFalse 4)
(const TLParen 5)
(const TRParen 6)
(const TEOS 7)
(const TERROR 8)

(struct Token
    (token (proc (n) (<= TIdentifier n TERROR)))
    (location Location?)
    (lexeme string?)
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
