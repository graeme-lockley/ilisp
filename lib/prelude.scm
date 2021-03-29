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

(const (not x) (if x #f #t))

; By loading module.scm into this file, the public procedures and macros are
; exported as a result of the module mechanism.  Note that this is an anomoly
; and is used to bootstrap the module mechanism.
(load-file "./module.scm")

(*builtin*.map-assoc! (*builtin*.pair-car **root**) :unit (module-import "./unit.scm"))

(import "./number.scm" :as Number)
(import "./sequence.scm" :as Sequence)

(const any Sequence.any)
(const car Sequence.car)
(const cdr Sequence.cdr)
(const count Sequence.count)
(const drop Sequence.drop)
(const drop-right Sequence.drop-right)
(const empty? Sequence.empty?)
(const ends-with Sequence.ends-with)
(const filter Sequence.filter)
(const fold Sequence.fold)
(const fold-right Sequence.fold-right)
(const nth Sequence.nth)
(const slice Sequence.slice)
(const starts-with Sequence.starts-with)
(const string->int Number.string->int)
(const take Sequence.take)
(const take-right Sequence.take-right)

(const- (type-of v)
  (if (*builtin*.null? v) "nil"
      (boolean? v) "boolean"
      (*builtin*.pair? v) "pair"
      (*builtin*.vector? v) "vector"
      (*builtin*.string? v) "string"
      (number? v) "number"
      (*builtin*.map? v) "map"
      (*builtin*.proc? v) "fn"
      (macro? v) "macro"
      (keyword? v) "keyword"
      (*builtin*.symbol? v) "symbol"
      "don't know"
  )
)
