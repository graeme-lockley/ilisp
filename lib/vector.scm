(export count (get (car **root**) :builtins 'vector-count))

(export (ends-with v prefix)
  (do
    (define (dec n) (- n 1))

    (define (iter v-idx prefix-idx)
        (cond 
            (< prefix-idx 0) 
                t

            (= (nth v v-idx) (nth prefix prefix-idx)) 
                (iter (dec v-idx) (dec prefix-idx))

            :else 
                f
        )
    )

    (cond 
        (> (count prefix) (count v)) 
            f

        :else 
            (iter (dec (count v)) (dec (count prefix)))
    )
  )  
)

(export filter (get (car **root**) :builtins 'vector-filter))

(export nth (get (car **root**) :builtins 'vector-nth))

(export (starts-with v prefix)
  (do
    (define (inc n) (+ n 1))

    (define (iter idx max)
        (cond 
            (= idx max) 
                t

            (= (nth v idx) (nth prefix idx)) 
                (iter (inc idx) max)

            :else 
                f
        )
    )

    (cond 
        (> (count prefix) (count v)) 
            f

        :else 
            (iter 0 (count prefix))
    )
  )  
)
