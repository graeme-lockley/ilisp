(export count *builtin*.string-count)

(export (drop v n)
    (slice v n (count v))
)

(export (drop-right v n)
    (slice v 0 (- (count v) n 1))
)

(export ends-with *builtin*.string-ends-with)

(export filter *builtin*.string-filter)

(export (fold v z p)
    (do
        (define (fold-idx idx z)
            (if (= idx (count v))
                    z
                (fold-idx (+ idx 1) (p z (nth v idx)))
            )
        )

        (fold-idx 0 z)
    )
)

(export (fold-right v z p)
    (do
        (define (iter idx z) 
            (if
                (= idx 0) 
                    z 
                (do
                    (define idxp (- idx 1))
                    (iter idxp (p (nth v idxp) z))
                )
            )
        )
        
        (iter (count v) z)
    )
)

(export nth *builtin*.string-nth)

(export reverse *builtin*.string-reverse)

(export slice *builtin*.string-slice)

(export starts-with *builtin*.string-starts-with)

(export (take s n)
    (slice s 0 (- n 1))
)

(export (take-right s n)
    (slice s (- (count s) n) (count s))
)