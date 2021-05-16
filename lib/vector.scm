(import "./builtin.scm" :as Builtin)

(const mutable-vector Builtin.mutable-vector)

(const count *builtin*.vector-count)

(const (drop v n)
    (slice v n (count v))
)

(const (drop-right v n)
    (slice v 0 (- (count v) n 1))
)

(const (ends-with v prefix)
    (const (dec n) (- n 1))

    (const (iter v-idx prefix-idx)
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

(const filter *builtin*.vector-filter)

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
        (if (= idx 0) 
                z 
            (do
                (const idxp (- idx 1))
                (iter idxp (p (nth v idxp) z))
            )
        )
    )
    
    (iter (count v) z)
)

(const (->mutable seq)
    (if (*builtin*.null? seq) (*builtin*.vector->mutable-vector [])
        (*builtin*.pair? seq) (*builtin*.list->mutable-vector seq)
        (*builtin*.vector->mutable-vector seq)
    )
)

(const nth *builtin*.vector-nth)

(const nth! *builtin*.vector-nth!)

(const range *builtin*.vector-range)

(const reverse *builtin*.vector-reverse)

(const slice *builtin*.vector-slice)

(const sort! *builtin*.vector-sort!)

(const (starts-with v prefix)
    (const (inc n) (+ n 1))

    (const (iter idx max)
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

(const (take v n)
    (slice v 0 (- n 1))
)

(const (take-right v n)
    (slice v (- (count v) n) (count v))
)
