(import "./frame.scm" :as Frame)

(export the-empty-environment ())

(export (enclosing-environment env) (cdr env))

(export (first-frame env) (car env))

(export (lookup-variable-value var env)
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