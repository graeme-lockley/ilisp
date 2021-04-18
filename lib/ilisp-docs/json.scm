(import "../list.scm" :as List)
(import "../string.scm" :as String)

; Converts an iLisp object into a JSON string.  Not all iLisp values can be 
; represented as a JSON string - when they are encountered the signal 
; `UnableToRepresentAsJson` is raised with the offending value.
;
; :usage (->string v)
;
; :parameter v value?
;   The value to be converted into a string.
;
; :returns string?
;   A JSON representation of `v` as a string.
;
; :signal UnableToRepresentAsJson
;   A component of `v` can not be represented in Json.
(const (->string v) 
    (if (*builtin*.string? v) (string->string v)
        (*builtin*.keyword? v) (keyword->string v)
        (*builtin*.symbol? v) (symbol->string v)
        (*builtin*.integer? v) (str v)
        (*builtin*.boolean? v) (boolean->string v)
        (List.list? v) (list->string v)
        (*builtin*.pair? v) (pair->string v)
        (*builtin*.vector? v) (vector->string v)
        (*builtin*.map? v) (map->string v)
        (raise 'UnableToRepresentAsJson v)
    )
)

(const- (string->string s)
    (const (special? c)
        (or (= c #x9) (= c #xa) (= c #xd))
    )

    (const (mark-up-character c)
        (if (= c #x9) "\\t"
            (= c #xa) "\\n"
            (= c #xd) ""
            c
        )
    )

    (const (mark-up-string s)
        (List.list->string (List.map (String.string->list s) mark-up-character))
    )

    (if (String.any s special?) (str "\"" (mark-up-string s) "\"")
        (pr-str s)
    )
)

(const- (keyword->string k)
    (string->string (str k))
)

(const- (symbol->string k)
    (string->string (str k))
)

(const- (boolean->string b)
    (if b "true" "false")
)

(const- (list->string v)
    (str "[" (String.interpolate-with (List.map v ->string) ", ") "]")
)

(const- (pair->string v)
    (const (to-list pair')
        (if (*builtin*.null? pair') ()
            (*builtin*.pair? pair') (pair (car pair') (to-list (cdr pair')))
            (pair pair' ())
        )
    )

    (list->string (to-list v))
)

(const- (vector->string v)
    (list->string (List.->list v))
)

(const- (map->string v)
    (const pairs (*builtin*.map->list v))

    (const (pair->string p)
        (str "\"" (car p) "\": " (->string (cdr p)))
    )

    (str "{" (String.interpolate-with (List.map pairs pair->string) ", ") "}")
)


; Parses the JSON string `s` and returning an iLisp representation of the
; underlying value.
;
; :usage (->value s)
;
; :parameter s string?
;   The JSON value represented as a string to be converted into an iLisp value.
;
; :returns value?
;   An iLisp value representing the passed JSON string.
;
; :signal SyntaxError
;   The string `s` is not a well formed JSON string respresentation.
(const (->value s) ())
