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
        (*builtin*.integer? v) (str v)
        (List.list? v) (list->string v)
        (*builtin*.vector? v) (vector->string v)
        (raise 'UnableToRepresentAsJson v)
    )
)

(const- (string->string s)
    (const (special? c)
        (= c #x9)
    )

    (const (mark-up-character c)
        (if (= c #x9) "\\t"
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

(const- (interpolate ss)
    (if (*builtin*.null? ss) ""
        (do (const h (car ss))
            (const r (cdr ss))

            (if (*builtin*.null? r)
                h
                (List.fold r h (proc (acc v) (str acc " " v)))
            )
        )
    )
)

(const- (list->string v)
    (str "[" (interpolate (List.map v ->string)) "]")
)

(const- (vector->string v)
    (list->string (List.->list v))
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
