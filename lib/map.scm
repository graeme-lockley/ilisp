(import "./list.scm" :as List)

(const- (flatten ls)
    (*builtin*.apply concat ls)
)

(const (union . ms)
    (*builtin*.apply 
        *builtin*.mk-map 
        (flatten
            (List.map 
                (flatten (List.map ms *builtin*.map->list)) 
                (proc (a) (list (car a) (cdr a)))
            )
        )
    )
)
