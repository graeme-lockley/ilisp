(import "./list.scm" :as List)
(import "./string.scm" :as String)
(import "./vector.scm" :as Vector)

(const (empty? seq)
  (if (nil? seq) #t
      (pair? seq) #f
      (vector? seq) (= (Vector.count seq) 0)
      (string? seq) (= seq "")
      (raise 'InvalidArgument {:procedure 'empty? :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (count seq)
  (if (nil? seq) 0
      (pair? seq) (List.count seq)
      (vector? seq) (Vector.count seq)
      (string? seq) (String.count seq)
      (raise 'InvalidArgument {:procedure 'count :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (starts-with seq prefix)
  (if (nil? seq) #f
      (pair? seq) (List.starts-with seq prefix)
      (vector? seq) (Vector.starts-with seq prefix)
      (string? seq) (String.starts-with seq prefix)
      (raise 'InvalidArgument {:procedure 'starts-with :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (ends-with seq prefix)
  (if (nil? seq) #f
      (pair? seq) (List.ends-with seq prefix)
      (vector? seq) (Vector.ends-with seq prefix)
      (string? seq) (String.ends-with seq prefix)
      (raise 'InvalidArgument {:procedure 'ends-with :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (filter seq predicate)
  (if (nil? seq) ()
      (pair? seq) (List.filter seq predicate)
      (vector? seq) (Vector.filter seq predicate)
      (string? seq) (String.filter seq predicate)
      (raise 'InvalidArgument {:procedure 'filter :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (nth seq n)
  (if (nil? seq) ()
      (pair? seq) (List.nth seq n)
      (vector? seq) (Vector.nth seq n)
      (string? seq) (String.nth seq n)
      (raise 'InvalidArgument {:procedure 'nth :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (slice seq from to)
  (if (nil? seq) ()
      (pair? seq) (List.slice seq from to)
      (vector? seq) (Vector.slice seq from to)
      (string? seq) (String.slice seq from to)
      (raise 'InvalidArgument {:procedure 'slice :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (take seq n)
  (if (nil? seq) ()
      (pair? seq) (List.take seq n)
      (vector? seq) (Vector.take seq n)
      (string? seq) (String.take seq n)
      (raise 'InvalidArgument {:procedure 'take :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (take-right seq n)
  (if (nil? seq) ()
      (pair? seq) (List.take-right seq n)
      (vector? seq) (Vector.take-right seq n)
      (string? seq) (String.take-right seq n)
      (raise 'InvalidArgument {:procedure 'take-right :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (drop seq n)
  (if (nil? seq) ()
      (pair? seq) (List.drop seq n)
      (vector? seq) (Vector.drop seq n)
      (string? seq) (String.drop seq n)
      (raise 'InvalidArgument {:procedure 'drop :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (drop-right seq n)
  (if (nil? seq) ()
      (pair? seq) (List.drop-right seq n)
      (vector? seq) (Vector.drop-right seq n)
      (string? seq) (String.drop-right seq n)
      (raise 'InvalidArgument {:procedure 'drop-right :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (fold seq z p)
  (if (nil? seq) z
      (pair? seq) (List.fold seq z p)
      (vector? seq) (Vector.fold seq z p)
      (string? seq) (String.fold seq z p)
      (raise 'InvalidArgument {:procedure 'fold :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (fold-right seq z p)
  (if (nil? seq) z
      (pair? seq) (List.fold-right seq z p)
      (vector? seq) (Vector.fold-right seq z p)
      (string? seq) (String.fold-right seq z p)
      (raise 'InvalidArgument {:procedure 'fold-right :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (index-of haystack needles)
    (if (not (pair? needles))
        (set! needles (list needles))
    )

    (const (find-needle idx)
        (const haystackp (drop haystack idx))

        (const r 
            (fold needles ()
                (proc (acc needle)
                    (if (not (nil? acc)) acc
                        (starts-with haystackp needle) (list idx needle)
                        ()
                    )
                )
            )
        )

        (if (empty? haystackp) ()
            (nil? r) (find-needle (+ idx 1))
            r
        )
    )

    (find-needle 0)
)

(const (split s seps)
    (const index-of-result (index-of s seps))

    (if (nil? index-of-result) (list s)
        (do
            (const index-of-sep (nth index-of-result 0))
            (const sep (nth index-of-result 1))

            (cons 
                (take s index-of-sep) 
                (split (drop s (+ index-of-sep (count sep))) seps)
            )
        )
    )
)

(const (sum ns)
    (fold ns 0 +)
)

(const (any seq f)
    (> (count (filter seq f)) 0)
)
