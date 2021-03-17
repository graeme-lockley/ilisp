(export count *builtin*.vector-count)

(export (drop v n)
    (slice v n (count v))
)

(export (drop-right v n)
    (slice v 0 (- (count v) n 1))
)

(export (ends-with v prefix)
  (do
    (define (dec n) (- n 1))

    (define (iter v-idx prefix-idx)
        (if (< prefix-idx 0) 
                #t
            (= (nth v v-idx) (nth prefix prefix-idx)) 
                (iter (dec v-idx) (dec prefix-idx))
            #f
        )
    )

    (if (> (count prefix) (count v)) 
            #f
        (iter (dec (count v)) (dec (count prefix)))
    )
  )  
)

(export filter *builtin*.vector-filter)

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
            (if (= idx 0) 
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

(export ->mutable *builtin*.vector-mutable)

(export nth *builtin*.vector-nth)

(export nth! *builtin*.vector-nth!)

(export range *builtin*.vector-range)

(export reverse *builtin*.vector-reverse)

(export slice *builtin*.vector-slice)

(export sort! *builtin*.vector-sort!)

(export (starts-with v prefix)
  (do
    (define (inc n) (+ n 1))

    (define (iter idx max)
        (if (= idx max) 
                #t
            (= (nth v idx) (nth prefix idx)) 
                (iter (inc idx) max)
            #f
        )
    )

    (if (> (count prefix) (count v)) 
            #f
        (iter 0 (count prefix))
    )
  )  
)

(export (take v n)
    (slice v 0 (- n 1))
)

(export (take-right v n)
    (slice v (- (count v) n) (count v))
)