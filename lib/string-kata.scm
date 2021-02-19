(import "./sequence.scm" :as Sequence)
(import "./unit.scm" :as Unit)

(define (char->int n)
    (if (<= 48 n 57) (- n 48)
        (raise 'InvalidArgument {:procedure 'char->int :arg-number 0 :expected "0..9" :received (char->string n)})
    )
)

(define (string->int n)
    (do
        (define (->int n)
            (fold n 0 
                (fn (acc d) 
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

(define (negative? n) (< n 0))

(export (add input)
    (do
        (define tokens 
            (if (starts-with input "//[")
                    (do
                        (define inputs (Sequence.split (drop input 3) "]\n"))

                        (Sequence.split (nth inputs 1) (Sequence.split (nth inputs 0) "]["))
                    )

                (starts-with input "//")
                    (Sequence.split (drop input 4) (slice input 2 2))

                (Sequence.split input '("," "\n"))
            )
        )
        (define numbers (map tokens string->int))

        (if (any numbers negative?)
            (raise (filter numbers negative?))
            (Sequence.sum (filter numbers (fn (n) (<= n 1000))))
        )
    )
)