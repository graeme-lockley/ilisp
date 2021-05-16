(macro (union name . parts)
    (const predicate-name (*builtin*.symbol (str "" name "?")))

    `(do
        (const (~name v)
            (if ((or? ~@parts) v) v
                (raise 'IllegalArgument {:procedure '~name :arg-number 0 :expected-type `(or ~@parts) :received v})
            )
        )

        (const (~predicate-name v)
            ((or? ~@parts) v)
        )
    )
)
