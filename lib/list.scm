(export count (get (car **root**) :builtins 'list-count))

(export filter (get (car **root**) :builtins 'list-filter))

(export nth (get (car **root**) :builtins 'list-nth))

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
