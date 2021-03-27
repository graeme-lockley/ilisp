(const (mk vars vals)
    (const (interleve vars' vals')
        (if (and (*builtin*.null? vars') (*builtin*.null? vals'))
                ()

            (or (*builtin*.null? vars') (*builtin*.null? vals'))
                (raise 'ListsNotSameLength {:procedure 'make-frame :vars vars :vals :vals})

            (*builtin*.cons (car vars') (*builtin*.cons (car vals') (interleve (cdr vars') (cdr vals'))))
        )
    )

    (apply *builtin*.mutable-map (interleve vars vals))
)

(const (frame-variables frame)
    (map (map->list frame) car)
)

(const (frame-values frame)
    (map (map->list frame) cdr)
)

(const (add-binding! frame var val)
    (*builtin*.map-assoc! frame var val)
)

(const (binding frame var)
    (if (contains? frame var)
        (*builtin*.cons var (map-get frame var))

        ()
    )
)