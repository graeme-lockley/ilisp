(const (mk vars vals)
    (const (interleve vars' vals')
        (if (and (nil? vars') (nil? vals'))
                ()

            (or (nil? vars') (nil? vals'))
                (raise 'ListsNotSameLength {:procedure 'make-frame :vars vars :vals :vals})

            (cons (car vars') (cons (car vals') (interleve (cdr vars') (cdr vals'))))
        )
    )

    (apply hash-map (interleve vars vals))
)

(const (frame-variables frame)
    (map (map->list frame) car)
)

(const (frame-values frame)
    (map (map->list frame) cdr)
)

(const (add-binding! frame var val)
    (assoc! frame var val)
)

(const (binding frame var)
    (if (contains? frame var)
        (cons var (get frame var))

        ()
    )
)