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

; Determine whether or not the passed value is an integer.
;
; :usage (integer? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to an integer otherwise `#f`.
(const integer? Builtin.integer?)
