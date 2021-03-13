(export (char->int n)
    (if (<= 48 n 57) (- n 48)
        (raise 'InvalidArgument {:procedure 'char->int :arg-number 0 :expected "0..9" :received (char->string n)})
    )
)

(export (string->int n)
    (do
        (define (->int n)
            (fold n 0 
                (proc (acc d) 
                    (+ (* 10 acc) (char->int d)) 
                )
            )
        )

        (if (starts-with n "-")
            (- (->int (drop n 1)))
            (->int n)
        )
    )
)

(export (negative? n) (< n 0))

(export (positive? n) (>= n 0))

(export (abs n) 
    (if (< n 0) (- n)
        n
    )
)

(export (modulo m n) (- m (* n (/ m n))))
