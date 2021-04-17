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

; Accepts a list of values and a separator and returns a list consisting of 
; each element from `ss` with `sep` in between.
;
; :usage (interpolate-with ss sep)
; :parameter ss list?
;   A list of elements
; :parameter sep any?
;   The element to be used as separator between `elements`
; :return list?
;
; :assert-equals (interpolate-with () ", ") ()
; :assert-equals (interpolate-with '(1) ", ") '(1)
; :assert-equals (interpolate-with '(1 2 3) ", ") '(1 ", " 2 "," 3)
(const (interpolate-with ss sep)
    (if (*builtin*.null? ss) ()
        (do (const h (car ss))
            (const r (cdr ss))

            (if (*builtin*.null? r)
                (list h)
                (*builtin*.pair h (*builtin*.pair sep (interpolate-with r sep)))
            )
        )
    )
)

(const (list? l)
    (if (*builtin*.null? l) #t
        (and (*builtin*.pair? l) (list? (cdr l)))
    )
)

(const (list->string l) (*builtin*.apply str l))

(const map *builtin*.list-map)

(const nth *builtin*.list-nth)

(const (reverse lst)
    (->list (Vector.reverse (*builtin*.list->vector lst)))
)

(const (separate lst sep)
    (if (*builtin*.null? lst) ()
        (*builtin*.null? (*builtin*.pair-cdr lst)) lst
        (do
            (const s (if (*builtin*.proc? sep) (sep) sep))

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
        (*builtin*.pair? s) s
        (*builtin*.vector? s) (*builtin*.vector->list s)
        (*builtin*.string? s) (fold-right s () pair)
        (raise 'InvalidArgument {:received s :expected-type (list 'pair 'vector () 'string) :arg-number 0 :procedure '->list})
    )
)
