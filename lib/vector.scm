(export count (get (car **root**) :builtins 'vector-count))
(export filter (get (car **root**) :builtins 'vector-filter))
(export nth (get (car **root**) :builtins 'vector-nth))

(export (starts-with v prefix)
  (do
    (define (iter idx max)
        (cond (= idx max) t
              (= (nth v idx) (nth prefix idx)) (iter (+ idx 1) max)
              :else f
        )
    )

    (cond (> (count prefix) (count v)) f
          :else (iter 0 (count prefix))
    )
  )  
)