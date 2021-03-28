(import "./frame.scm" :as Frame)

(const the-empty-environment ())

(const (enclosing-environment env) (*builtin*.pair-cdr env))

(const (first-frame env) (*builtin*.pair-car env))

(const (lookup-variable-value env var)
    (const (env-lookup env)
        (if (= env the-empty-environment)
                (raise 'UnboundVariable {:procedure 'lookup-variable-name :name var})

            (do (const frame (first-frame env))
                (const binding (Frame.binding frame var))

                (if (*builtin*.null? binding)
                    (env-lookup (enclosing-environment env))
                    (*builtin*.pair-cdr binding)
                )
            )
        )
    )

    (env-lookup env)
)

(const (set-variable-value! env var val)
    (const (env-set! env)
        (if (= env the-empty-environment)
                (raise 'UnboundVariable {:procedure 'set-variable-value! :name var})

            (do (const frame (first-frame env))
                (const binding (Frame.binding frame var))

                (if (*builtin*.null? binding)
                    (env-set! (enclosing-environment env))
                    (Frame.add-binding! frame var val)
                )
            )
        )
    )

    (env-set! env)
)

(const (define-variable! env var val)
    (const frame (first-frame env))

    (Frame.add-binding! frame var val)
)

(const (extend base-env vars vals)
    (*builtin*.pair (Frame.mk vars vals) base-env)
)
