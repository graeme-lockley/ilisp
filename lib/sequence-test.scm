(import "./unit.scm" :as Unit)

(Unit.test "count"
  (Unit.assert-equals (count [1 2 3 4]) 4)
  (Unit.assert-equals (count ()) 0)
  (Unit.assert-equals (count '(1 2 3 4 5 6)) 6)
  (Unit.assert-equals (count "hello world") 11)

  (Unit.assert-signal (count 123) (proc (signal) (do
    (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
    (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
    (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) 'count)
  )))
)

(Unit.test "drop"
  (Unit.assert-equals (drop [1 2 3 4] 2) [3 4])

  (Unit.assert-equals (drop '(1 2 3 4 5 6) 2) '(3 4 5 6))
  
  (Unit.assert-equals (drop "hello world" 2) "llo world")
  
  (Unit.assert-signal (drop 123 2) (proc (signal) (do
    (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
    (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
    (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) 'drop)
  )))
)

(Unit.test "drop-right"
  (Unit.assert-equals (drop-right [1 2 3 4] 2) [1 2])

  (Unit.assert-equals (drop-right '(1 2 3 4 5 6) 2) '(1 2 3 4))
  
  (Unit.assert-equals (drop-right "hello world" 2) "hello wor")
  
  (Unit.assert-signal (drop-right 123 2) (proc (signal) (do
    (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
    (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
    (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) 'drop-right)
  )))
)

(Unit.test "empty?"
  (Unit.assert-truthy (empty? ()))
  (Unit.assert-falsy (empty? '(1)))

  (Unit.assert-truthy (empty? []))
  (Unit.assert-falsy (empty? [1 2 3]))

  (Unit.assert-truthy (empty? ""))
  (Unit.assert-falsy (empty? "hello"))

  (Unit.assert-signal (empty? 123) (proc (signal) (do
    (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
    (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
    (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) 'empty?)
  )))
)

(Unit.test "ends-with"
  (Unit.assert-truthy (ends-with [1 2 3 4] [3 4]))
  (Unit.assert-falsy (ends-with [1 2 3 4] [2 3]))

  (Unit.assert-truthy (ends-with '(1 2 3 4 5 6) '(4 5 6)))
  (Unit.assert-falsy (ends-with '(1 2 3 4 5 6) '(3 4 5)))

  (Unit.assert-truthy (ends-with "hello world" "world"))
  (Unit.assert-falsy (ends-with "hello world" "worl"))

  (Unit.assert-signal (ends-with 123 1) (proc (signal) (do
    (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
    (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
    (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) 'ends-with)
  )))
)

(Unit.test "filter"
  (const (odd n) (= (- n (* (/ n 2) 2)) 1))

  (Unit.assert-equals (filter [1 2 3 4] odd) [1 3])
  (Unit.assert-equals (filter '(1 2 3 4 5 6) odd) '(1 3 5))
  (Unit.assert-equals (filter "hello world" odd) "eowo")

  (Unit.assert-signal (filter 123 1) 
    (proc (signal)
      (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) 'filter)
    )
  )
)

(Unit.test "nth"
  (Unit.assert-equals (nth [1 2 3 4] 2) 3)
  (Unit.assert-equals (nth '(1 2 3 4 5 6) 2) 3)
  (Unit.assert-equals (nth "hello world" 2) #\l)

  (Unit.assert-signal (nth 123 1) 
    (proc (signal)
      (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) 'nth)
    )
  )
)

(Unit.test "slice"
  (Unit.assert-equals (slice [1 2 3 4] 1 2) [2 3])

  (Unit.assert-equals (slice '(1 2 3 4 5 6) 1 2) '(2 3))
  
  (Unit.assert-equals (slice "hello world" 1 2) "el")
  
  (Unit.assert-signal (slice 123 1 2) 
    (proc (signal)
      (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) 'slice)
    )
  )
)

(Unit.test "starts-with"
  (Unit.assert-truthy (starts-with [1 2 3 4] [1 2]))
  (Unit.assert-falsy (starts-with [1 2 3 4] [1 2 4]))

  (Unit.assert-truthy (starts-with '(1 2 3 4 5 6) '(1 2 3)))
  (Unit.assert-falsy (starts-with '(1 2 3 4 5 6) '(1 2 4)))

  (Unit.assert-truthy (starts-with "hello world" "hello"))
  (Unit.assert-falsy (starts-with "hello world" "helos"))

  (Unit.assert-signal (starts-with 123 1) 
    (proc (signal)
      (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) 'starts-with)
    )
  )
)

(Unit.test "take"
  (Unit.assert-equals (take [1 2 3 4] 2) [1 2])

  (Unit.assert-equals (take '(1 2 3 4 5 6) 2) '(1 2))
  
  (Unit.assert-equals (take "hello world" 2) "he")
  
  (Unit.assert-signal (take 123 2) 
    (proc (signal)
      (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) 'take)
    )
  )
)

(Unit.test "take-right"
  (Unit.assert-equals (take-right [1 2 3 4] 2) [3 4])

  (Unit.assert-equals (take-right '(1 2 3 4 5 6) 2) '(5 6))
  
  (Unit.assert-equals (take-right "hello world" 2) "ld")
  
  (Unit.assert-signal (take-right 123 2) 
    (proc (signal)
      (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
      (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) 'take-right)
    )
  )
)
