(const- *top-level* :t)

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
      (vector? v) "vector"
      (string? v) "string"
      (number? v) "number"
      (*builtin*.map? v) "map"
      (fn? v) "fn"
      (macro? v) "macro"
      (keyword? v) "keyword"
      (symbol? v) "symbol"
      "don't know"
  )
)
