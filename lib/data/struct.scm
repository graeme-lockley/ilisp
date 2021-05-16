(import "../list.scm" :names list?)
(import "../predicate.scm" :names or?)

(macro (struct name . fields)
    (const selector-name (*builtin*.symbol (str name "-symbol-name")))
    (const predicate-name (*builtin*.symbol (str "" name "?")))
    (import "../list.scm" :names map-idx)

    (map-idx fields (proc (field idx)
        (if (or (not (list? field)) 
                (not (= (count field) 2))
                (not (*builtin*.symbol? (car field)))
            )
                (raise 'IllegalArgument {:procedure name :arg-number idx :expected-type `(list-fixed-of? symbol? ~(nth field 1)) :received field})
        )
    ))

    `(do
        (const- ~selector-name (*builtin*.symbol (str *source-name* "-" '~name)))

        (const (~name ~@(*builtin*.list-map fields car))
            ~@(map-idx fields (proc (field idx)
                (const n (nth field 0))
                (const t (nth field 1))

                `(if (not (~t ~n)) (raise 'IllegalArgument {:procedure '~name :arg-number ~idx :expected-type '~t :name '~n :received ~n}))
            ))

            [~selector-name ~@(*builtin*.list-map fields car)]
        )

        ~@(map-idx fields (proc (field idx)
            (const field-name (*builtin*.symbol (str name "-" (car field))))
            `(const (~field-name v)
                (nth v ~(+ idx 1))
            )
        ))

        (const (~predicate-name v)
            (and (*builtin*.vector? v)
                 (= (count v) ~(+ 1 (count fields)))
                 (= (nth v 0) ~selector-name)
                 ~@(map-idx fields (proc (field idx)
                    `(~(nth field 1) (nth v ~(+ idx 1)))
                 ))
            )
        )
    )
)

(macro (mutable-struct name . fields)
    (const selector-name (*builtin*.symbol (str name "-symbol-name")))
    (const predicate-name (*builtin*.symbol (str "" name "?")))
    (import "../list.scm" :names map-idx)
    (import "../vector.scm" :names mutable-vector nth!)

    (map-idx fields (proc (field idx)
        (if (or (not (list? field)) 
                (not (= (count field) 2))
                (not (*builtin*.symbol? (car field)))
            )
                (raise 'IllegalArgument {:procedure name :arg-number idx :expected-type `(list-fixed-of? symbol? ~(nth field 1)) :received field})
        )
    ))

    `(do
        (const- ~selector-name (*builtin*.symbol (str *source-name* "-" '~name)))

        (const (~name ~@(*builtin*.list-map fields car))
            ~@(map-idx fields (proc (field idx)
                (const n (nth field 0))
                (const t (nth field 1))

                `(if (not (~t ~n)) (raise 'IllegalArgument {:procedure '~name :arg-number ~idx :expected-type '~t :name '~n :received ~n}))
            ))

            (*builtin*.mutable-vector ~selector-name ~@(*builtin*.list-map fields car))
        )

        ~@(map-idx fields (proc (field idx)
            (const field-name (*builtin*.symbol (str name "-" (car field))))
            `(const (~field-name v)
                (nth v ~(+ idx 1))
            )
        ))

        ~@(map-idx fields (proc (field idx)
            (const field-name (*builtin*.symbol (str name "-" (car field) "!")))
            (const n (nth field 0))
            (const t (nth field 1))

            `(const (~field-name v s)
                (if (~t s) (*builtin*.vector-nth! v ~(+ idx 1) s)
                    (raise 'IllegalArgument {:procedure '~name :arg-number ~idx :expected-type '~t :name '~n :received s})
                )
            )
        ))

        (const (~predicate-name v)
            (and (*builtin*.vector? v)
                 (= (count v) ~(+ 1 (count fields)))
                 (= (nth v 0) ~selector-name)
                 ~@(map-idx fields (proc (field idx)
                    `(~(nth field 1) (nth v ~(+ idx 1)))
                 ))
            )
        )
    )
)
