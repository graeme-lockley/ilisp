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

(export-macro (cond . xs)
  (if (> (count xs) 0) 
    (list 
      'if 
      (first xs) 
      (if (> (count xs) 1) 
        (nth xs 1) 
        (raise "odd number of forms to cond")) 
      (cons 'cond (rest (rest xs))))
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

(import "./list.scm" :as List)
(import "./string.scm" :as String)
(import "./vector.scm" :as Vector)

(export (empty? seq)
  (if (nil? seq) t
      (pair? seq) f
      (vector? seq) (= (Vector.count seq) 0)
      (string? seq) (= seq "")
      (raise 'InvalidArgument {:procedure 'empty? :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (count seq)
  (if (nil? seq) 0
      (pair? seq) (List.count seq)
      (vector? seq) (Vector.count seq)
      (string? seq) (String.count seq)
      (raise 'InvalidArgument {:procedure 'count :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (starts-with seq prefix)
  (if (nil? seq) f
      (pair? seq) (List.starts-with seq prefix)
      (vector? seq) (Vector.starts-with seq prefix)
      (string? seq) (String.starts-with seq prefix)
      (raise 'InvalidArgument {:procedure 'starts-with :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (ends-with seq prefix)
  (if (nil? seq) f
      (pair? seq) (List.ends-with seq prefix)
      (vector? seq) (Vector.ends-with seq prefix)
      (string? seq) (String.ends-with seq prefix)
      (raise 'InvalidArgument {:procedure 'ends-with :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (filter seq predicate)
  (if (nil? seq) f
      (pair? seq) (List.filter seq predicate)
      (vector? seq) (Vector.filter seq predicate)
      (string? seq) (String.filter seq predicate)
      (raise 'InvalidArgument {:procedure 'filter :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (nth seq n)
  (if (nil? seq) ()
      (pair? seq) (List.nth seq n)
      (vector? seq) (Vector.nth seq n)
      (string? seq) (String.nth seq n)
      (raise 'InvalidArgument {:procedure 'nth :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (slice seq from to)
  (if (nil? seq) ()
      (pair? seq) (List.slice seq from to)
      (vector? seq) (Vector.slice seq from to)
      (string? seq) (String.slice seq from to)
      (raise 'InvalidArgument {:procedure 'slice :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (take seq n)
  (if (nil? seq) ()
      (pair? seq) (List.take seq n)
      (vector? seq) (Vector.take seq n)
      (string? seq) (String.take seq n)
      (raise 'InvalidArgument {:procedure 'take :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (take-right seq n)
  (if (nil? seq) ()
      (pair? seq) (List.take-right seq n)
      (vector? seq) (Vector.take-right seq n)
      (string? seq) (String.take-right seq n)
      (raise 'InvalidArgument {:procedure 'take-right :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (drop seq n)
  (if (nil? seq) ()
      (pair? seq) (List.drop seq n)
      (vector? seq) (Vector.drop seq n)
      (string? seq) (String.drop seq n)
      (raise 'InvalidArgument {:procedure 'drop :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (drop-right seq n)
  (if (nil? seq) ()
      (pair? seq) (List.drop-right seq n)
      (vector? seq) (Vector.drop-right seq n)
      (string? seq) (String.drop-right seq n)
      (raise 'InvalidArgument {:procedure 'drop-right :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (fold seq z p)
  (if (nil? seq) z
      (pair? seq) (List.fold seq z p)
      (vector? seq) (Vector.fold seq z p)
      (string? seq) (String.fold seq z p)
      (raise 'InvalidArgument {:procedure 'fold :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

(export (fold-right seq z p)
  (if (nil? seq) z
      (pair? seq) (List.fold-right seq z p)
      (vector? seq) (Vector.fold-right seq z p)
      (string? seq) (String.fold-right seq z p)
      (raise 'InvalidArgument {:procedure 'fold-right :arg-number 0 :expected-type (list 'pair () 'vector 'string) :received seq})
  )
)

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
