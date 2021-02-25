(assoc! (car **root**)
  'is-top-level     (mo ()           '(nil? (cdr **scope**)))

  'define-procedure (mo (sig body)   `(do (assoc! (car **scope**) '~(car sig) (fn ~(cdr sig) ~body)) ()))
  'define-atom      (mo (name value) `(do (assoc! (car **scope**) '~name ~value) ()))
  'define-macro     (mo (sig body)   `(do (assoc! (car **scope**) '~(car sig) (mo ~(cdr sig) ~body)) ()))
  'define           (mo (sig body)   `(if (list? '~sig) (define-procedure ~sig ~body) (define-atom ~sig ~body)))

  'export-procedure (mo (sig body)   `(if (is-top-level) (define-procedure '~sig '~body) (do (assoc! (car (cdr **scope**)) '~(car sig) (fn ~(cdr sig) ~body)) ())))
  'export-atom      (mo (name value) `(if (is-top-level) (define-atom '~name '~value) (do (assoc! (car (cdr **scope**)) '~name ~value) ())))
  'export-macro     (mo (sig body)   `(if (is-top-level) (define-macro '~sig '~body) (do (assoc! (car (cdr **scope**)) '~(car sig) (mo ~(cdr sig) ~body)) ())))
  'export           (mo (sig body)   `(if (list? '~sig) (export-procedure ~sig ~body) (export-atom ~sig ~body)))
)

(export-macro (and . terms)
  (if (nil? terms) 
    t
    (if (= (count terms) 1)
      (first terms) 
      `(if ~(first terms) 
        (and ~@(rest terms)) 
        ()
      )
    )
  )
)

(export-macro (or . terms)
  (if (nil? terms) 
    () 
    (if (= (count terms) 1)
      (first terms)
      `(if ~(first terms) 
        t
        (or ~@(rest terms)) 
      )
    )
  )
)

; A wrapper macro that uses the builtin set! quoting the first parameter in so
; that is appears as a symbol.
(export-macro (set! name value)
  `((get (car **root**) :builtins 'set!) '~name ~value)
)

(export *source-name* (str (get **env** 'PWD) "/home"))

; Replace the builtin load-file with a macro which uses the surrounding context 
; to access *source-name*.  This value is used to capture relative library 
; names. 
(export-macro (load-file f)
  `(do
    (assoc! (car **scope**) '*source-name* ((get (car **root**) :builtins 'file-name-relative-to-file-name) *source-name* ~f))
    (eval (read-string (str "(do " (slurp *source-name*) "\n)") *source-name*))
  )
)

(export t (=))
(export f ())

(export (not x) (if x f t))

; By loading package.scm into this file, the public procedures and macros are
; exported as a result of the package mechanism.  Note that this is an anomoly
; and is used to bootstrap the package mechanism.
(load-file "./package.scm")

(import "./number.scm" :as Number)
(import "./sequence.scm" :as Sequence)

(export any Sequence.any)
(export count Sequence.count)
(export drop Sequence.drop)
(export drop-right Sequence.drop-right)
(export empty? Sequence.empty?)
(export ends-with Sequence.ends-with)
(export filter Sequence.filter)
(export fold Sequence.fold)
(export fold-right Sequence.fold-right)
(export nth Sequence.nth)
(export slice Sequence.slice)
(export starts-with Sequence.starts-with)
(export string->int Number.string->int)
(export take Sequence.take)
(export take-right Sequence.take-right)

(define (type-of v)
  (if (nil? v) "nil"
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
