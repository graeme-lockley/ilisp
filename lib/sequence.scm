(import "./byte-vector.scm" :as ByteVector)
(import "./list.scm" :as List)
(import "./string.scm" :as String)
(import "./vector.scm" :as Vector)

(const (car seq)
  (if (*builtin*.pair? seq) (*builtin*.pair-car seq)
      (*builtin*.vector? seq) (*builtin*.vector-nth seq 0)
      (*builtin*.string? seq) (*builtin*.string-nth seq 0)
      (raise 'InvalidArgument {:procedure 'car :arg-number 0 :expected-type (list 'pair 'vector 'string) :received seq})
  )
)

(const (cdr seq)
  (if (*builtin*.pair? seq) (*builtin*.pair-cdr seq)
      (*builtin*.vector? seq) (*builtin*.vector-slice seq 1 (*builtin*.vector-count seq))
      (*builtin*.string? seq) (*builtin*.string-slice seq 1 (*builtin*.string-count seq))
      (raise 'InvalidArgument {:procedure 'cdr :arg-number 0 :expected-type (list 'pair 'vector 'string) :received seq})
  )
)

(const (empty? seq)
  (if (*builtin*.null? seq) #t
      (*builtin*.pair? seq) #f
      (*builtin*.vector? seq) (= (Vector.count seq) 0)
      (*builtin*.string? seq) (= seq "")
      (raise 'InvalidArgument {:procedure 'empty? :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (count seq)
  (if (*builtin*.null? seq) 0
      (*builtin*.pair? seq) (List.count seq)
      (*builtin*.vector? seq) (Vector.count seq)
      (*builtin*.string? seq) (String.count seq)
      (*builtin*.byte-vector? seq) (*builtin*.byte-vector-count seq)
      (raise 'InvalidArgument {:procedure 'count :arg-number 0 :expected-type (list 'pair () 'vector 'string 'byte-vector) :received seq})
  )
)

(const (starts-with seq prefix)
  (if (*builtin*.null? seq) #f
      (*builtin*.pair? seq) (List.starts-with seq prefix)
      (*builtin*.vector? seq) (Vector.starts-with seq prefix)
      (*builtin*.string? seq) (String.starts-with seq prefix)
      (raise 'InvalidArgument {:procedure 'starts-with :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (ends-with seq prefix)
  (if (*builtin*.null? seq) #f
      (*builtin*.pair? seq) (List.ends-with seq prefix)
      (*builtin*.vector? seq) (Vector.ends-with seq prefix)
      (*builtin*.string? seq) (String.ends-with seq prefix)
      (raise 'InvalidArgument {:procedure 'ends-with :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (filter seq predicate)
  (if (*builtin*.null? seq) ()
      (*builtin*.pair? seq) (List.filter seq predicate)
      (*builtin*.vector? seq) (Vector.filter seq predicate)
      (*builtin*.string? seq) (String.filter seq predicate)
      (raise 'InvalidArgument {:procedure 'filter :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (nth seq n)
  (if (*builtin*.null? seq) ()
      (*builtin*.pair? seq) (List.nth seq n)
      (*builtin*.vector? seq) (Vector.nth seq n)
      (*builtin*.string? seq) (String.nth seq n)
      (*builtin*.byte-vector? seq) (ByteVector.nth seq n)
      (raise 'InvalidArgument {:procedure 'nth :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (slice seq from to)
  (if (*builtin*.null? seq) ()
      (*builtin*.pair? seq) (List.slice seq from to)
      (*builtin*.vector? seq) (Vector.slice seq from to)
      (*builtin*.string? seq) (String.slice seq from to)
      (raise 'InvalidArgument {:procedure 'slice :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (take seq n)
  (if (*builtin*.null? seq) ()
      (*builtin*.pair? seq) (List.take seq n)
      (*builtin*.vector? seq) (Vector.take seq n)
      (*builtin*.string? seq) (String.take seq n)
      (raise 'InvalidArgument {:procedure 'take :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (take-right seq n)
  (if (*builtin*.null? seq) ()
      (*builtin*.pair? seq) (List.take-right seq n)
      (*builtin*.vector? seq) (Vector.take-right seq n)
      (*builtin*.string? seq) (String.take-right seq n)
      (raise 'InvalidArgument {:procedure 'take-right :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (drop seq n)
  (if (*builtin*.null? seq) ()
      (*builtin*.pair? seq) (List.drop seq n)
      (*builtin*.vector? seq) (Vector.drop seq n)
      (*builtin*.string? seq) (String.drop seq n)
      (raise 'InvalidArgument {:procedure 'drop :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (drop-right seq n)
  (if (*builtin*.null? seq) ()
      (*builtin*.pair? seq) (List.drop-right seq n)
      (*builtin*.vector? seq) (Vector.drop-right seq n)
      (*builtin*.string? seq) (String.drop-right seq n)
      (raise 'InvalidArgument {:procedure 'drop-right :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (fold seq z p)
  (if (*builtin*.null? seq) z
      (*builtin*.pair? seq) (List.fold seq z p)
      (*builtin*.vector? seq) (Vector.fold seq z p)
      (*builtin*.string? seq) (String.fold seq z p)
      (raise 'InvalidArgument {:procedure 'fold :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (fold-right seq z p)
  (if (*builtin*.null? seq) z
      (*builtin*.pair? seq) (List.fold-right seq z p)
      (*builtin*.vector? seq) (Vector.fold-right seq z p)
      (*builtin*.string? seq) (String.fold-right seq z p)
      (raise 'InvalidArgument {:procedure 'fold-right :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(const (index-of haystack needles)
    (const needles'
        (if (*builtin*.pair? needles) needles
            (list needles)
        )
    )

    (const (find-needle idx)
        (const haystackp (drop haystack idx))

        (const r 
            (fold needles' ()
                (proc (acc needle)
                    (if (not (*builtin*.null? acc)) acc
                        (starts-with haystackp needle) (list idx needle)
                        ()
                    )
                )
            )
        )

        (if (empty? haystackp) ()
            (*builtin*.null? r) (find-needle (+ idx 1))
            r
        )
    )

    (find-needle 0)
)

(const (split s seps)
    (const index-of-result (index-of s seps))

    (if (*builtin*.null? index-of-result) (list s)
        (do
            (const index-of-sep (nth index-of-result 0))
            (const sep (nth index-of-result 1))

            (*builtin*.pair 
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
