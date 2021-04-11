(import "./character.scm" :as Character)

(const count *builtin*.string-count)

(const (drop v n)
    (slice v n (count v))
)

(const (drop-right v n)
    (slice v 0 (- (count v) n 1))
)

(const (drop-right-while s p)
    (const s-count (count s))

    (const (gobble idx)
        (if (<= idx 0) ""
            (p (nth s (- idx 1))) (gobble (- idx 1))
            (slice s 0 (- idx 1))
        )
    )

    (gobble s-count)
)

(const (drop-while s p)
    (const s-count (count s))

    (const (gobble idx)
        (if (>= idx s-count) ""
            (p (nth s idx)) (gobble (+ idx 1))
            (slice s idx s-count)
        )
    )

    (gobble 0)
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

(const (take-while s p)
    (const s-count (count s))

    (const (gobble idx)
        (if (>= idx s-count) s
            (p (nth s idx)) (gobble (+ idx 1))
            (slice s 0 (- idx 1))
        )
    )

    (gobble 0)
)

(const (trim s)
    (trim-left (trim-right s))
)

(const (trim-left s)
    (drop-while s Character.whitespace?)
)

(const (trim-right s)
    (drop-right-while s Character.whitespace?)
)
