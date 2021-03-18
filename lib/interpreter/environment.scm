(import "./frame.scm" :as Frame)

(const the-empty-environment ())

(const (enclosing-environment env) (cdr env))

(const (first-frame env) (car env))

(const (lookup-variable-value env var)
    (const (env-lookup env)
        (if (= env the-empty-environment)
                (raise 'UnboundVariable {:procedure 'lookup-variable-name :name var})

            (do (const frame (first-frame env))
                (const binding (Frame.binding frame var))

                (if (nil? binding)
                    (env-lookup (enclosing-environment env))
                    (cdr binding)
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

                (if (nil? binding)
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
    (cons (Frame.mk vars vals) base-env)
)
