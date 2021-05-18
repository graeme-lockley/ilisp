; Converts the digit in the range `#\0` to `#\9` to the numeric value `0` to `9` respectively.
;
; :usage (character->number c)
; :parameter c character?
; :returns number?
; :signal InvalidArgument
;   `c` is not in the required range
(const (character->number n)
    (if (<= 48 n 57) (- n 48)
        (raise 'InvalidArgument {:procedure 'character->number :arg-number 0 :expected "0..9" :received (*builtin*.character->string n)})
    )
)

(const (string->number n)
    (const (->int n)
        (fold n 0 
            (proc (acc d) 
                (+ (* 10 acc) (character->number d)) 
            )
        )
    )

    (if (starts-with n "-")
        (- (->int (drop n 1)))
        (->int n)
    )
)

(const (negative? n) (< n 0))

(const (positive? n) (>= n 0))

(const (abs n) 
    (if (< n 0) (- n)
        n
    )
)

; Returns `n` incremented by 1.
;
; :usage (inc n)
; :parameter n integer?
; :returns integer?
(const (inc n)
    (+ n 1)
)

; Returns `n` decremented by 1.
;
; :usage (dec n)
; :parameter n integer?
; :returns integer?
(const (dec n)
    (- n 1)
)

(const (modulo m n) (- m (* n (/ m n))))
