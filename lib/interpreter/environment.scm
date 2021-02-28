(import "./frame.scm" :as Frame)

(export the-empty-environment ())

(export (enclosing-environment env) (cdr env))

(export (first-frame env) (car env))

(export (lookup-variable-value env var)
    (do (define (env-lookup env)
            (if (= env the-empty-environment)
                    (raise 'UnboundVariable {:procedure 'lookup-variable-name :name var})

                (do (define frame (first-frame env))
                    (define binding (Frame.binding frame var))

                    (if (nil? binding)
                        (env-lookup (enclosing-environment env))
                        (cdr binding)
                    )
                )
            )
        )

        (env-lookup env)
    )
)

(export (set-variable-value! env var val)
    (do (define (env-set! env)
            (if (= env the-empty-environment)
                    (raise 'UnboundVariable {:procedure 'set-variable-value! :name var})

                (do (define frame (first-frame env))
                    (define binding (Frame.binding frame var))

                    (if (nil? binding)
                        (env-set! (enclosing-environment env))
                        (Frame.add-binding! frame var val)
                    )
                )
            )
        )

        (env-set! env)
    )
)

(export (define-variable! env var val)
    (do (define frame (first-frame env))

        (Frame.add-binding! frame var val)
    )
)

(export (extend base-env vars vals)
    (cons (Frame.mk vars vals) base-env)
)
