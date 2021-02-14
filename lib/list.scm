(import "./vector.scm" :as Vector)

(export count (get (car **root**) :builtins 'list-count))

(export drop (get (car **root**) :builtins 'list-drop))

(export (ends-with lst suffix)
    (starts-with (reverse lst) (reverse suffix))
)

(export filter (get (car **root**) :builtins 'list-filter))

(export nth (get (car **root**) :builtins 'list-nth))

(export (reverse lst)
    (to-list (Vector.reverse (vec lst)))
)

(export (slice lst start end)
    (cond
        (<= start 0)
            (take lst (+ end 1))

        :else
            (take (drop lst start) (+ (- end start) 1))
    )
)

(export (starts-with lst prefix)
    (cond
        (nil? prefix)
            t

        (nil? lst)
            f

        :else
            (and (= (car lst) (car prefix)) (starts-with (cdr lst) (cdr prefix)))
    )
)

(export take (get (car **root**) :builtins 'list-take))

(export (to-list s)
    (cond
        (nil? s)
            s

        (list? s)
            s

        (or (vector? s) (string? s))
            (map s (fn (n) n))

        :else
            (raise 'InvalidArgument {:received s :expected-type (list 'pair 'vector () 'string) :arg-number 0 :procedure 'to-list})
    )
)
