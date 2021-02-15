(import "./vector.scm" :as Vector)

(export count (get (car **root**) :builtins 'list-count))

(export drop (get (car **root**) :builtins 'list-drop))

(export (drop-right lst n)
    (->list (Vector.drop-right (vec lst) n))
)

(export (ends-with lst suffix)
    (starts-with (reverse lst) (reverse suffix))
)

(export filter (get (car **root**) :builtins 'list-filter))

 (export (fold xs z f)
    (if (nil? xs) z
        (fold (cdr xs) (f z (car xs)) f)
    )
)

(export (fold-right xs z f)
    (if (nil? xs) z
        (f (car xs) (fold-right (cdr xs) z f))
    )
)

(export nth (get (car **root**) :builtins 'list-nth))

(export (reverse lst)
    (->list (Vector.reverse (vec lst)))
)

(export (slice lst start end)
    (if (<= start 0) (take lst (+ end 1))
        (take (drop lst start) (+ (- end start) 1))
    )
)

(export (starts-with lst prefix)
    (if (nil? prefix) t
        (nil? lst) f
        (and (= (car lst) (car prefix)) (starts-with (cdr lst) (cdr prefix)))
    )
)

(export take (get (car **root**) :builtins 'list-take))

(export (take-right lst n)
    (->list (Vector.take-right (vec lst) n))
)

(export (->list s)
    (if (nil? s) s
        (list? s) s
        (or (vector? s) (string? s)) (map s (fn (n) n))
        (raise 'InvalidArgument {:received s :expected-type (list 'pair 'vector () 'string) :arg-number 0 :procedure '->list})
    )
)
