(import "./vector.scm" :as Vector)

(const count *builtin*.list-count)

(const drop *builtin*.list-drop)

(const (drop-right lst n)
    (->list (Vector.drop-right (*builtin*.list->vector lst) n))
)

(const (ends-with lst suffix)
    (starts-with (reverse lst) (reverse suffix))
)

(const filter *builtin*.list-filter)

 (const (fold xs z f)
    (if (*builtin*.null? xs) z
        (fold (*builtin*.pair-cdr xs) (f z (*builtin*.pair-car xs)) f)
    )
)

(const (fold-right xs z f)
    (if (*builtin*.null? xs) z
        (f (*builtin*.pair-car xs) (fold-right (*builtin*.pair-cdr xs) z f))
    )
)

(const nth *builtin*.list-nth)

(const (reverse lst)
    (->list (Vector.reverse (*builtin*.list->vector lst)))
)

(const (separate lst sep)
    (if (*builtin*.null? lst) ()
        (*builtin*.null? (*builtin*.pair-cdr lst)) lst
        (do
            (const s (if (fn? sep) (sep) sep))

            (*builtin*.pair (*builtin*.pair-car lst) (*builtin*.pair s (separate (*builtin*.pair-cdr lst) sep)))
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
        (and (= (*builtin*.pair-car lst) (*builtin*.pair-car prefix)) (starts-with (*builtin*.pair-cdr lst) (*builtin*.pair-cdr prefix)))
    )
)

(const take *builtin*.list-take)

(const (take-right lst n)
    (->list (Vector.take-right (*builtin*.list->vector lst) n))
)

(const (->list s)
    (if (*builtin*.null? s) s
        (list? s) s
        (or (vector? s) (string? s)) (map s (proc (n) n))
        (raise 'InvalidArgument {:received s :expected-type (list 'pair 'vector () 'string) :arg-number 0 :procedure '->list})
    )
)
