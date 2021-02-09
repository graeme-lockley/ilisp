(export count (get (car **root**) :builtins 'vector-count))

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
        (cond 
            (< prefix-idx 0) 
                t

            (= (nth v v-idx) (nth prefix prefix-idx)) 
                (iter (dec v-idx) (dec prefix-idx))

            :else 
                f
        )
    )

    (cond 
        (> (count prefix) (count v)) 
            f

        :else 
            (iter (dec (count v)) (dec (count prefix)))
    )
  )  
)

(export filter (get (car **root**) :builtins 'vector-filter))

(export (fold v z p)
    (do
        (define (fold-idx idx z)
            (cond
                (= idx (count v))
                    z

                :else
                    (fold-idx (+ idx 1) (p z (nth v idx)))
            )
        )

        (fold-idx 0 z)
    )
)

(export (fold-right v z p)
    (do
        (define (iter idx z) 
            (cond 
                (= idx 0) 
                    z 
                    
                :else
                    (do
                        (define idxp (- idx 1))
                        (iter idxp (p (nth v idxp) z))
                    )
            )
        )
        
        (iter (count v) z)
    )
)

(export mutable (get (car **root**) :builtins 'vector-mutable))

(export nth (get (car **root**) :builtins 'vector-nth))

(export nth! (get (car **root**) :builtins 'vector-nth!))

(export range (get (car **root**) :builtins 'vector-range))

(export reverse (get (car **root**) :builtins 'vector-reverse))

(export slice (get (car **root**) :builtins 'vector-slice))

(export (starts-with v prefix)
  (do
    (define (inc n) (+ n 1))

    (define (iter idx max)
        (cond 
            (= idx max) 
                t

            (= (nth v idx) (nth prefix idx)) 
                (iter (inc idx) max)

            :else 
                f
        )
    )

    (cond 
        (> (count prefix) (count v)) 
            f

        :else 
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