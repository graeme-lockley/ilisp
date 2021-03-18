(const (char->int n)
    (if (<= 48 n 57) (- n 48)
        (raise 'InvalidArgument {:procedure 'char->int :arg-number 0 :expected "0..9" :received (char->string n)})
    )
)

(const (string->int n)
    (const (->int n)
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

(const (negative? n) (< n 0))

(const (positive? n) (>= n 0))

(const (abs n) 
    (if (< n 0) (- n)
        n
    )
)

(const (modulo m n) (- m (* n (/ m n))))
