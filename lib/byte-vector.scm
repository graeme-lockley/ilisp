(import "./builtin.scm" :as Builtin)

; Converts a string to a byte-vector.
;
; :usage (string->byte-vector s)
; :parameter s string?
;   The string to be converted
; :returns byte-vector?
;   Note that the lenghts of `s` and the result are the same.
; :signal InvalidArgument
;   The argument `s` is not a string
(const string->byte-vector Builtin.string->byte-vector)

(const nth Builtin.byte-vector-nth)

(const (slice bv start end)
    (const (runner idx)
        (if (< idx end) (pair (nth bv idx) (runner (+ idx 1)))
            ()
        )
    )

    (runner start)
)

(const (slice->string bv start end)
    (*builtin*.apply str (slice bv start end))
)
