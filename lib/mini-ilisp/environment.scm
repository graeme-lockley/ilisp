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

(const (define-binding! env name value)
    (Map.assoc! (car env) name value)
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

(const (get env name)
    (if (null? env) ()
        (in-scope? env name) (map-get (car env) name)
        (get (close-scope env) name)
    )
)
