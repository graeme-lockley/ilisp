(import "./number.scm" :as Number)
(import "./sequence.scm" :as Sequence)
(import "./unit.scm" :as Unit)

(export (add input)
    (do
        (define tokens 
            (if (starts-with input "//[")
                    (do
                        (define inputs (Sequence.split (drop input 3) "]\n"))

                        (Sequence.split (nth inputs 1) (Sequence.split (nth inputs 0) "]["))
                    )

                (starts-with input "//")
                    (Sequence.split (drop input 4) (slice input 2 2))

                (Sequence.split input '("," "\n"))
            )
        )
        (define numbers (map tokens string->int))

        (if (any numbers Number.negative?)
            (raise (filter numbers Number.negative?))
            (Sequence.sum (filter numbers (fn (n) (<= n 1000))))
        )
    )
)