(export count (get (car **root**) :builtins 'string-count))

(export (drop v n)
    (slice v n (count v))
)

(export (drop-right v n)
    (slice v 0 (- (count v) n 1))
)

(export ends-with (get (car **root**) :builtins 'string-ends-with))

(export filter (get (car **root**) :builtins 'string-filter))

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

(export nth (get (car **root**) :builtins 'string-nth))

(export reverse (get (car **root**) :builtins 'string-reverse))

(export slice (get (car **root**) :builtins 'string-slice))

(export starts-with (get (car **root**) :builtins 'string-starts-with))

(export (take s n)
    (slice s 0 (- n 1))
)

(export (take-right s n)
    (slice s (- (count s) n) (count s))
)