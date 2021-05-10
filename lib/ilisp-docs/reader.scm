(import "../character.scm" :as Character)
(import "../list.scm" :as List)
; (import "../sequence.scm" :as Sequence)
(import "../string.scm" :as String)

(const- @ *builtin*.atom-dereference)
(const- @! *builtin*.atom-swap!)
(const- (@= a v) (*builtin*.atom-swap! a (proc (x) v)))

(const- (drop-while s p)
    (const s-count (count s))

    (const (gobble idx)
        (if (>= idx s-count) ()
            (p (nth s idx)) (gobble (+ idx 1))
            (slice s idx s-count)
        )
    )

    (gobble 0)
)

(const- (drop-right-while s p)
    (const s-count (count s))

    (const (gobble idx)
        (if (= idx 0) ()
            (p (nth s (- idx 1))) (gobble (- idx 1))
            (slice s 0 (- idx 1))
        )
    )

    (gobble s-count)
)

(const- (take-while s p)
    (const s-count (count s))

    (const (gobble idx)
        (if (>= idx s-count) s
            (p (nth s idx)) (gobble (+ idx 1))
            (slice s 0 (- idx 1))
        )
    )

    (gobble 0)
)

; Parses the property descriptions by running through `lines` and gobbling all 
; lines until it reaches a line that starts with a colon.  A colon indicates
; the start of a new property.
;
; :usage (parse-property-description lines)
; :parameter lines (list-of? string?)
;   A list of strings where each string is the literal of what was parsed from the source file.
; :returns (pair? (list-of? string?) (list-of? string?))
;   The first element of the result is a list of the property's description with 
;   the second element the remainder of `lines` with the property descriptions
;   having been removed.
(const (parse-property-description lines)
    (if (*builtin*.null? lines) (pair () ())
        (do (const line (car lines))

            (if (starts-with line ":") (pair () lines)
                (do (const rest (parse-property-description (cdr lines)))
                    (if (and (*builtin*.null? (car rest)) (= "" line))
                        (pair () (cdr rest))
                        (pair (pair line (car rest)) (cdr rest))
                    )
                )
            )
        )
    )
)

(const (parse-property-comments lines)
    (const (strip-left-whitespace lines)
        (const (calculate-left-indent line)
            (const (loop idx)
                (if (Character.whitespace? (nth line idx)) (loop (+ idx 1)) idx)
            )

            (if (= line "") 0 (loop 0))
        )

        (const (min values)
            (if (*builtin*.null? values) 0
                (fold (cdr values) (car values) (proc (a b) (if (< a b) a b)))
            )
        )

        (const left-indent (min (List.map lines calculate-left-indent)))

        (if (> left-indent 0)
            (List.map lines (proc (line) (drop line left-indent)))
            lines
        )
    )

    (if (*builtin*.null? lines) '(())
        (do (const line (car lines))
            (const first-word (take-while line Character.not-whitespace?))

            (if (= first-word ":parameter")
                    (do (const rest-line (String.trim-left (drop-while line Character.not-whitespace?)))
                        (const name (take-while rest-line Character.not-whitespace?))
                        (const signature (String.trim-left (drop-while rest-line Character.not-whitespace?)))
                        (const remainder (parse-property-description (cdr lines)))
                        (const other-properties (parse-property-comments (cdr remainder)))
                        
                        (pair (pair {'type "parameter" 'name name 'signature signature 'description (strip-left-whitespace (car remainder))} (car other-properties)) (cdr other-properties))
                    )

                (do (const signature (String.trim (drop-while line Character.not-whitespace?)))
                    (const remainder (parse-property-description (cdr lines)))
                    (const other-properties (parse-property-comments (cdr remainder)))

                    (pair (pair {'type (drop first-word 1) 'signature signature 'description (strip-left-whitespace (car remainder))} (car other-properties)) (cdr other-properties))
                )
            )
        )
    )
)

(const (parse-comments name comments)
    (const (procedure-description? line) (not (starts-with line ":")))

    (const property-comments
        (car (parse-property-comments (drop-while comments procedure-description?)))
    )

    {   'name name
        'description (drop-right-while (take-while comments procedure-description?) (proc (l) (= l "")))
        'properties property-comments
    }
)

(const- (drop-semicolon line)
    (if (starts-with line "; ") (drop line 2) (drop line 1))
)

(const (parse content)
    (let state 'IN-BETWEEN)
    (let items ())
    (let comments ())

    (for-each (Sequence.split content "\x0a;")
        (proc (line)
            (if (starts-with line ";")
                    (if (= (@ state) 'IN-BETWEEN)
                            (do (@= state 'IN-DOC)
                                (@= comments (pair (drop-semicolon line) ()))
                            )

                        (= (@ state) 'IN-DOC)
                            (@! comments (proc (c) (pair (drop-semicolon line) c)))
                    )

                (starts-with line "(const ")
                    (do (const start-idx (if (starts-with line "(const (") 8 7))
                        (const rest-of-line (drop line start-idx))
                        (const name (take-while rest-of-line (proc (c) (not (or (= c #x20) (= c #x29))))))   
                        (@! items (proc (l) (pair (parse-comments name (List.reverse (@ comments))) l)))
                        (@= state 'IN-BETWEEN)
                        (@= comments ())
                    )

                (do (@= state 'IN-BETWEEN)
                    (@= comments ())
                )
            )
        )
    )

    (List.reverse (@ items))
)

;; (const c
;;     (parse (*builtin*.slurp (*builtin*.file-name-relative-to-file-name *source-name* "../builtin.scm")))
;; )

;; (println c)
