(import "./vector.scm" :as Vector)

(export count (get (car **root**) :builtins 'list-count))

(export (ends-with lst suffix)
    (starts-with (reverse lst) (reverse suffix))
)

(export filter (get (car **root**) :builtins 'list-filter))

(export nth (get (car **root**) :builtins 'list-nth))

(export (reverse lst)
    (map (Vector.reverse (vec lst)) (fn (n) n))
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
