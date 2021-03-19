(const- *top-level* :t)

(macro (and . terms)
  (if (nil? terms) 
    #t
    (if (= ((get *builtin* 'list-count) terms) 1)
      (first terms) 
      `(if ~(first terms) 
        (and ~@(rest terms)) 
        #f
      )
    )
  )
)

(macro (or . terms)
  (if (nil? terms) 
    #f
    (if (= ((get *builtin* 'list-count) terms) 1)
      (first terms)
      `(if ~(first terms) 
        #t
        (or ~@(rest terms)) 
      )
    )
  )
)

; A wrapper macro that uses the builtin set! quoting the first parameter in so
; that is appears as a symbol.
(macro (set! name value)
  `((get *builtin* 'set!) '~name ~value)
)

(const *source-name* (str (get **env** 'PWD) "/home"))

; Replace the builtin load-file with a macro which uses the surrounding context 
; to access *source-name*.  This value is used to capture relative library 
; names. 
(set! load-file
  (mo (f) `(do
    (assoc! (car **scope**) '*source-name* ((get *builtin* 'file-name-relative-to-file-name) *source-name* ~f))
    (eval (read-string (str "(do " (slurp *source-name*) "\n)") *source-name*))
  ))
)

(const (not x) (if x #f #t))

; By loading package.scm into this file, the public procedures and macros are
; exported as a result of the package mechanism.  Note that this is an anomoly
; and is used to bootstrap the package mechanism.
(load-file "./package.scm")

(assoc! (car **root**) :unit (package-import "./unit.scm"))

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
  (if (nil? v) "nil"
      (boolean? v) "boolean"
      (pair? v) "pair"
      (vector? v) "vector"
      (string? v) "string"
      (number? v) "number"
      (map? v) "map"
      (fn? v) "fn"
      (macro? v) "macro"
      (keyword? v) "keyword"
      (symbol? v) "symbol"
      "don't know"
  )
)
