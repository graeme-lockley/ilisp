(import "./list.scm" :as List)
(import "./number.scm" :as Number)
(import "./sequence.scm" :as Sequence)

(const (add input)
    (const tokens 
        (if (starts-with input "//[")
                (do (const inputs (Sequence.split (drop input 3) "]\n"))
                    (Sequence.split (nth inputs 1) (List.reverse (List.sort (Sequence.split (nth inputs 0) "]["))))
                )

            (starts-with input "//")
                (Sequence.split (drop input 4) (slice input 2 2))

            (Sequence.split input '("," "\n"))
        )
    )
    (const numbers (*builtin*.list-map tokens string->number))

    (if (any numbers Number.negative?)
            (raise (filter numbers Number.negative?))
        (Sequence.sum (filter numbers (proc (n) (<= n 1000))))
    )
)