(import "./vector.scm" :as Vector)

(const count *builtin*.list-count)

(const drop *builtin*.list-drop)

(const (drop-right lst n)
    (->list (Vector.drop-right (vec lst) n))
)

(const (ends-with lst suffix)
    (starts-with (reverse lst) (reverse suffix))
)

(const filter *builtin*.list-filter)

 (const (fold xs z f)
    (if (*builtin*.null? xs) z
        (fold (cdr xs) (f z (car xs)) f)
    )
)

(const (fold-right xs z f)
    (if (*builtin*.null? xs) z
        (f (car xs) (fold-right (cdr xs) z f))
    )
)

(const nth *builtin*.list-nth)

(const (reverse lst)
    (->list (Vector.reverse (vec lst)))
)

(const (separate lst sep)
    (if (*builtin*.null? lst) ()
        (*builtin*.null? (cdr lst)) lst
        (do
            (const s (if (fn? sep) (sep) sep))

            (*builtin*.pair (car lst) (*builtin*.pair s (separate (cdr lst) sep)))
        )
    )
)

(const (slice lst start end)
    (if (<= start 0) (take lst (+ end 1))
        (take (drop lst start) (+ (- end start) 1))
    )
)

(const (sort lst)
    (->list (Vector.sort! (Vector.->mutable lst)))
)

(const (starts-with lst prefix)
    (if (*builtin*.null? prefix) #t
        (*builtin*.null? lst) #f
        (and (= (car lst) (car prefix)) (starts-with (cdr lst) (cdr prefix)))
    )
)

(const take *builtin*.list-take)

(const (take-right lst n)
    (->list (Vector.take-right (vec lst) n))
)

(const (->list s)
    (if (*builtin*.null? s) s
        (list? s) s
        (or (vector? s) (string? s)) (map s (proc (n) n))
        (raise 'InvalidArgument {:received s :expected-type (list 'pair 'vector () 'string) :arg-number 0 :procedure '->list})
    )
)
