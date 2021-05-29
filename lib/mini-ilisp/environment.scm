(import "../map.scm" :as Map)

(const (empty)
    (open-scope ())
)

(const (open-scope env) 
    (pair (Map.mutable) env)
)

(const (close-scope env)
    (cdr env)
)

(const (define-binding! env name)
    (Map.assoc! (car env) name name)
)

(const (in-scope? env name)
    (Map.contains? (car env) name)
)

(const (binding? env name)
    (if (null? env) #f
        (in-scope? env name) #t
        (binding? (close-scope env) name)
    )
)