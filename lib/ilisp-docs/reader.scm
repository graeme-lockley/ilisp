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
; :parameter lines (list? string?)
; :returns (pair? (list? string?) (list? string?))
;   The first element of the result is a list of the property's description with 
;   the second element the remainder of `lines` with the property descriptions
;   having been removed.
(const (parse-property-description lines)
    (if (*builtin*.null? lines) (pair () ())
        (do (const line (car lines))

            (if (starts-with line ":") (pair () lines)
                (do (const rest (parse-property-description (cdr lines)))
                    (pair (pair line (car rest)) (cdr rest))
                )
            )
        )
    )
)

(const (parse-property-comments lines)
    (if (*builtin*.null? lines) '(())
        (do (const line (car lines))
            (const first-word (take-while line Character.not-whitespace?))

            (if (= first-word ":parameter")
                    (do (const rest-line (String.trim-left (drop-while line Character.not-whitespace?)))
                        (const name (take-while rest-line Character.not-whitespace?))
                        (const signature (String.trim-left (drop-while rest-line Character.not-whitespace?)))
                        (const remainder (parse-property-description (cdr lines)))
                        (const other-properties (parse-property-comments (cdr remainder)))
                        
                        (pair (pair {'type "parameter" 'name name 'signature signature 'description (car remainder)} (car other-properties)) (cdr other-properties))
                    )

                (do (const signature (String.trim (drop-while line Character.not-whitespace?)))
                    (const remainder (parse-property-description (cdr lines)))
                    (const other-properties (parse-property-comments (cdr remainder)))

                    (pair (pair {'type (drop first-word 1) 'signature signature 'description (car remainder)} (car other-properties)) (cdr other-properties))
                )
            )
        )
    )
)

(const (parse-comments name comments)
    (const (procedure-description? line) (not (starts-with line ":")))

    (const property-comments
        (const pc (parse-property-comments (drop-while comments procedure-description?)))
        (if (= '(()) pc) () pc)
    )

    {   'name name
        'description (take-while comments procedure-description?)
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
            ;; (println state " " line)
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
                        (const name (take-while rest-of-line (proc (c) (not (= c #x20)))))
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