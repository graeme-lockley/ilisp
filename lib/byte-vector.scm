(import "./builtin.scm" :as Builtin)
(import "./character.scm" :names ->character)

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

(const- (identity v) v)

(const- (slice-map bv f start end)
    (const (runner idx)
        (if (< idx end) (pair (f (nth bv idx)) (runner (+ idx 1)))
            ()
        )
    )

    (runner start)
)

(const (slice bv start end)
    (slice-map bv identity start end)
)

(const (slice->string bv start end)
    (const (runner idx)
        (if (< idx end) (pair (->character (nth bv idx)) (runner (+ idx 1)))
            ()
        )
    )

    (*builtin*.apply str (slice-map bv ->character start end))
)
