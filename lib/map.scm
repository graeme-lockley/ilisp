(import "./builtin.scm" :as Builtin)
(import "./list.scm" :as List)

; Mutate `map` by incorporating the passed bindings.
; 
; :usage (map! map . k1 v1 ... kn vn)
; :parameter map mutable-map?
; :parameter k1 any?
;   The key of the first binding.
; :parameter v1 any?
;   The value of the first binding.
; :parameter kn any?
;   The key of the last binding.
; :parameter vn any?
;   The value of the last binding.
(const assoc! Builtin.map-assoc!)

; Construct a mutable map from the passed arguments.
;
; :usage (mutable . values)
; :parameter values list?
; :returns mutable-map?
;   A mutable map composed of elements from `values`.
(const mutable Builtin.mutable-map)

(const contains? Builtin.map-contains?)

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
