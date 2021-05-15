(import "./builtin.scm" :as Builtin)

(const- (constant v) (proc (_) v))

; Determine whether or not the passed value is a value.
;
; :usage (any? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   Always returns `#t` given that all values in `iLisp` are, well, values.
(const any? (constant #t))

; Determine whether or not the passed value is null.
;
; :usage (null? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers is null otherwise `#f`.
(const null? Builtin.null?)

; Determine whether or not the passed value is an integer.
;
; :usage (integer? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to an integer otherwise `#f`.
(const integer? Builtin.integer?)

; Negates the result from the passed predicate.
;
; :usage (not? p)
; :parameter p proc?
;   A predicate
; :returns proc?
;   A predicate which accepts `any?` and returns `boolean?`.
(const (not? p)
    (proc (v) (not (p v)))
)

; Determines if at least one of the passed predicates, when applied to the 
; passed value, returns `#t`.  Should none of the passed predicates return `#t`
; then the procedure returns `#f`.
;
; :usage (or? . p)
; :parameter p (list-of? proc?)
;   A list of procedures where each procedure accepts an `any?` argument and returns 
;   `boolean?`.
; :returns proc?
;   A predicate which accepts `any?` and returns `boolean?`.
(const (or? . p)
    (proc (v)
        (const (runner p)
            (if (null? p) #f
                ((car p) v) #t
                (runner (cdr p))
            )
        )
        
        (runner p)
    )
)
