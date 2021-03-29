(const (mk vars vals)
    (const (interleve vars' vals')
        (if (and (*builtin*.null? vars') (*builtin*.null? vals'))
                ()

            (or (*builtin*.null? vars') (*builtin*.null? vals'))
                (raise 'ListsNotSameLength {:procedure 'make-frame :vars vars :vals :vals})

            (*builtin*.pair (*builtin*.pair-car vars') (*builtin*.pair (*builtin*.pair-car vals') (interleve (*builtin*.pair-cdr vars') (*builtin*.pair-cdr vals'))))
        )
    )

    (*builtin*.apply *builtin*.mutable-map (interleve vars vals))
)

(const (frame-variables frame)
    (*builtin*.list-map (map->list frame) car)
)

(const (frame-values frame)
    (*builtin*.list-map (map->list frame) cdr)
)

(const (add-binding! frame var val)
    (*builtin*.map-assoc! frame var val)
)

(const (binding frame var)
    (if (*builtin*.map-contains? frame var)
        (*builtin*.pair var (map-get frame var))

        ()
    )
)
