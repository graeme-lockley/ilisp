(const- *top-level* :t)

(const (first seq)
  (if (*builtin*.null? seq) ()
      (*builtin*.pair? seq) (*builtin*.pair-car seq)
      (*builtin*.vector? seq) (*builtin*.vector-nth seq 0)
      (*builtin*.string? seq) 
        (if (= (*builtin*.string-count seq) 0)
          ()
          (*builtin*.string-nth seq 0)
        )
  )
)

(const (rest seq)
  (if (*builtin*.null? seq) ()
      (*builtin*.pair? seq) (*builtin*.pair-cdr seq)
      (*builtin*.vector? seq) (*builtin*.vector-slice seq 1 (*builtin*.vector-count seq))
      (*builtin*.string? seq) 
        (if (= (*builtin*.string-count seq) 0)
          ()
          (*builtin*.string-slice seq 1 (*builtin*.string-count seq))
        )
  )
)

(macro (and . terms)
  (if (*builtin*.null? terms) 
    #t
    (if (= ((map-get *builtin* 'list-count) terms) 1)
      (first terms) 
      `(if ~(first terms) 
        (and ~@(rest terms)) 
        #f
      )
    )
  )
)

(macro (or . terms)
  (if (*builtin*.null? terms) 
    #f
    (if (= ((map-get *builtin* 'list-count) terms) 1)
      (first terms)
      `(if ~(first terms) 
        #t
        (or ~@(rest terms)) 
      )
    )
  )
)

(const number? *builtin*.integer?)

(const (sequential? s)
  (or
    (*builtin*.null? s)
    (*builtin*.pair? s)
    (*builtin*.vector? s)
  )
)

(const (not x) (if x #f #t))

; By loading module.scm into this file, the public procedures and macros are
; exported as a result of the module mechanism.  Note that this is an anomoly
; and is used to bootstrap the module mechanism.
(load-file "./module.scm")

(*builtin*.map-assoc! (*builtin*.pair-car **root**) :unit (module-import "./unit.scm"))

(import "./number.scm" :names 
  string->number
)

(import "./sequence.scm" :names 
  any 
  car 
  cdr 
  count 
  drop 
  drop-right 
  empty? 
  ends-with 
  filter 
  fold 
  fold-right 
  nth 
  slice 
  starts-with 
  take 
  take-right
)

(const identity (proc (n) n))
(const (constant c) (proc (n) c))

(import "./predicate.scm" :names
  null?
  not?
  or?
)

(const (<| . fs)
    (const (apply-rest f' v)
        (if (null? f') v
            ((car f') (apply-rest (cdr f') v))
        )
    )

    (if (null? fs) identity
        (proc (v)
            ((car fs) (apply-rest (cdr fs) v))
        )
    )
)

(const (|> . fs)
    (const (apply-rest fs' v)
        (if (null? fs') v
            (apply-rest (cdr fs') ((car fs') v))
        )
    )

    (proc (v)
        (apply-rest fs v)
    )
)

(const caar (<| car car))
(const cadr (<| car cdr))
(const cdar (<| cdr car))
(const cddr (<| cdr cdr))

(const (type-of v)
  (if (*builtin*.null? v) "nil"
      (*builtin*.boolean? v) "boolean"
      (*builtin*.pair? v) "pair"
      (*builtin*.vector? v) "vector"
      (*builtin*.string? v) "string"
      (*builtin*.integer? v) "integer"
      (*builtin*.map? v) "map"
      (*builtin*.proc? v) "fn"
      (*builtin*.macro? v) "macro"
      (*builtin*.keyword? v) "keyword"
      (*builtin*.symbol? v) "symbol"
      "don't know"
  )
)
