(import "./string.scm" :as String)
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

(define (sum ns)
    (fold ns 0 +)
)

(define (index-of haystack needles)
    (do
        (if (not (pair? needles))
            (set! needles (list needles))
        )

        (define (find-needle idx)
            (do
                (define haystackp (drop haystack idx))

                (define r 
                    (fold needles ()
                        (fn (acc needle)
                            (if (not (nil? acc)) acc
                                (starts-with haystackp needle) (list idx needle)
                                ()
                            )
                        )
                    )
                )

                (if (empty? haystackp) ()
                    (nil? r) (find-needle (+ idx 1))
                    r
                )
            )
        )

        (find-needle 0)
    )
)

(define (split s seps)
    (do
        (define index-of-result (index-of s seps))

        (if (nil? index-of-result) (list s)
            (do
                (define index-of-sep (nth index-of-result 0))
                (define sep (nth index-of-result 1))

                (cons 
                    (take s index-of-sep) 
                    (split (drop s (+ index-of-sep (count sep))) seps)
                )
            )
        )
    )
)

(define (negative? n) (< n 0))

(define (any seq f)
    (> (count (filter seq f)) 0)
)

(define (add input)
    (do
        (define tokens 
            (if (starts-with input "//[")
                    (do
                        (define inputs (split (drop input 3) "]\n"))

                        (split (nth inputs 1) (split (nth inputs 0) "]["))
                    )

                (starts-with input "//")
                    (split (drop input 4) (slice input 2 2))

                (split input '("," "\n"))
            )
        )
        (define numbers (map tokens string->int))

        (if (any numbers negative?)
            (raise (filter numbers negative?))
            (sum (filter numbers (fn (n) (<= n 1000))))
        )
    )
)

(Unit.test "given a blank should return 0"
    (Unit.assert-equals (add "") 0)
)

(Unit.test "given a number should return it's value if less then 1001 else 9"
    (Unit.assert-equals (add "123") 123)
    (Unit.assert-equals (add "1001") 0)
)

(Unit.test "given numbers separated with a comma should return the sum of all less than 1001"
    (Unit.assert-equals (add "1,2,10012,3") 6)
)

(Unit.test "given numbers separated with a comma or newline should return the sum of all less than 1001"
    (Unit.assert-equals (add "1\n2,3") 6)
)

(Unit.test "given numbers separated with a single character custom separator should return the sum of all less than 1001"
    (Unit.assert-equals (add "//;\n1;2;2001;3") 6)
    (Unit.assert-equals (add "//*\n1*2*3*3001*4") 10)
)

(Unit.test "given numbers separated with a single multi-character custom separator should return the sum of all less than 1001"
    (Unit.assert-equals (add "//[***]\n1***2***2001***3") 6)
)

(Unit.test "given numbers separated with multiple multi-character custom separators should return the sum of all less than 1001"
    (Unit.assert-equals (add "//[***][===]\n1***2===2001***3") 6)
)

(Unit.test "given numbers with at least one negative should report an error with all of the negatives"
    (Unit.assert-signal (add "0,-1,2,-3,4,-5,6")
        (fn (n)
            (Unit.assert-equals n (list (- 1) (- 3) (- 5)))
        )
    )
)

(println (Unit.stats))