(const count *builtin*.string-count)

(const (drop v n)
    (slice v n (count v))
)

(const (drop-right v n)
    (slice v 0 (- (count v) n 1))
)

(const ends-with *builtin*.string-ends-with)

(const filter *builtin*.string-filter)

(const (fold v z p)
    (const (fold-idx idx z)
        (if (= idx (count v))
                z
            (fold-idx (+ idx 1) (p z (nth v idx)))
        )
    )

    (fold-idx 0 z)
)

(const (fold-right v z p)
    (const (iter idx z) 
        (if
            (= idx 0) 
                z 
            (do
                (const idxp (- idx 1))
                (iter idxp (p (nth v idxp) z))
            )
        )
    )
    
    (iter (count v) z)
)

(const nth *builtin*.string-nth)

(const reverse *builtin*.string-reverse)

(const slice *builtin*.string-slice)

(const starts-with *builtin*.string-starts-with)

(const (take s n)
    (slice s 0 (- n 1))
)

(const (take-right s n)
    (slice s (- (count s) n) (count s))
)