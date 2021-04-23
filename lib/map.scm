(import "./list.scm" :as List)

(const (union . ms)
    (*builtin*.apply 
        *builtin*.mk-map 
        (*builtin*.apply 
            concat 
            (List.map 
                (*builtin*.apply 
                    concat 
                    (List.map ms *builtin*.map->list)
                ) 
                (proc (a) (list (car a) (cdr a)))
            )
        )
    )
)
