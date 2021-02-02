(assoc! (car **root**)
  'is-top-level     (mo ()           '(nil? (cdr **scope**)))

  'define-procedure (mo (sig body)   `(assoc! (car **scope**) '~(car sig) (fn ~(cdr sig) ~body)))
  'define-atom      (mo (name value) `(assoc! (car **scope**) '~name ~value))
  'define-macro     (mo (sig body)   `(assoc! (car **scope**) '~(car sig) (mo ~(cdr sig) ~body)))
  'define           (mo (sig body)   `(if (list? '~sig) (define-procedure ~sig ~body) (define-atom ~sig ~body)))

  'export-procedure (mo (sig body)   `(if (is-top-level) (define-procedure '~sig '~body) (assoc! (car (cdr **scope**)) '~(car sig) (fn ~(cdr sig) ~body))))
  'export-atom      (mo (name value) `(if (is-top-level) (define-atom '~name '~value) (assoc! (car (cdr **scope**)) '~name ~value)))
  'export-macro     (mo (sig body)   `(if (is-top-level) (define-macro '~sig '~body) (assoc! (car (cdr **scope**)) '~(car sig) (mo ~(cdr sig) ~body))))
  'export           (mo (sig body)   `(if (list? '~sig) (export-procedure ~sig ~body) (export-atom ~sig ~body)))
)

(export-macro (and . terms)
  `(if (nil? '~terms) 
    1 
    (if ~(first terms) 
      (and ~@(rest terms)) 
      ()
    )
  )
)

; (Unit.assert-equals (and) 1)
; (Unit.assert-equals (and 1) 1)
; (Unit.assert-equals (and 0) 0)
; (Unit.assert-equals (and 1 1) 1)
; (Unit.assert-equals (and 1 0) 0)
; (Unit.assert-equals (and 0 (/ 1 0)) 0)

(export-macro (or . terms)
  `(if (nil? '~terms) 
    () 
    (if ~(first terms) 
      1
      (or ~@(rest terms)) 
    )
  )
)

; (Unit.assert-equals (or) 0)
; (Unit.assert-equals (or 1) 1)
; (Unit.assert-equals (or 0) 0)
; (Unit.assert-equals (or 1 (/ 1 0)) 1)
; (Unit.assert-equals (or 0 1) 1)
; (Unit.assert-equals (or 0 0) 0)

(load-file "./lib/package.scm");

; (define Package (library-import "./lib/package.scm"))

(import "./lib/package.scm" :as Package)

(export library-import (library-use Package 'library-import))
(export library-use (library-use Package 'library-use))


