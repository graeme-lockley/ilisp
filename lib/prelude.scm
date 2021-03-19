(assoc! (car **root**)
  'is-top-level     (mo ()           '(nil? (cdr **scope**)))

  'define-procedure (mo (sig body)   `(do (assoc! (car **scope**) '~(car sig) (proc ~(cdr sig) ~body)) ()))
  'define-atom      (mo (name value) `(do (assoc! (car **scope**) '~name ~value) ()))
  'define-macro     (mo (sig body)   `(do (assoc! (car **scope**) '~(car sig) (mo ~(cdr sig) ~body)) ()))
  'define           (mo (sig body)   `(if (list? '~sig) (define-procedure ~sig ~body) (define-atom ~sig ~body)))

  'export-procedure (mo (sig body)   `(if (is-top-level) (define-procedure '~sig '~body) (do (assoc! (car (cdr **scope**)) '~(car sig) (proc ~(cdr sig) ~body)) ())))
  'export-atom      (mo (name value) `(if (is-top-level) (define-atom '~name '~value) (do (assoc! (car (cdr **scope**)) '~name ~value) ())))
  'export-macro     (mo (sig body)   `(if (is-top-level) (define-macro '~sig '~body) (do (assoc! (car (cdr **scope**)) '~(car sig) (mo ~(cdr sig) ~body)) ())))
  'export           (mo (sig body)   `(if (list? '~sig) (export-procedure ~sig ~body) (export-atom ~sig ~body)))
)

(export-macro (and . terms)
  (if (nil? terms) 
    #t
    (if (= (count terms) 1)
      (first terms) 
      `(if ~(first terms) 
        (and ~@(rest terms)) 
        #f
      )
    )
  )
)

(export-macro (or . terms)
  (if (nil? terms) 
    #f
    (if (= (count terms) 1)
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
(export-macro (set! name value)
  `((get *builtin* 'set!) '~name ~value)
)

(const- *top-level* :t)
(const *source-name* (str (get **env** 'PWD) "/home"))

; Replace the builtin load-file with a macro which uses the surrounding context 
; to access *source-name*.  This value is used to capture relative library 
; names. 
(export-macro (load-file f)
  `(do
    (assoc! (car **scope**) '*source-name* ((get *builtin* 'file-name-relative-to-file-name) *source-name* ~f))
    (eval (read-string (str "(do " (slurp *source-name*) "\n)") *source-name*))
  )
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
