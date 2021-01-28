(map-set! (car **root**) 'define-procedure (mo (sig body)   `(map-set! (car **scope**) '~(car sig) (fn ~(cdr sig) ~body))))
(map-set! (car **root**) 'define-atom      (mo (name value) `(map-set! (car **scope**) '~name ~value)))
(map-set! (car **root**) 'define-macro     (mo (sig body)   `(map-set! (car **scope**) '~(car sig) (mo ~(cdr sig) ~body))))
(map-set! (car **root**) 'define           (mo (sig body)   `(if (list? '~sig) (define-procedure ~sig ~body) (define-atom ~sig ~body))))

(define (list . x) x)
(define (load-file name) (eval (read-string (str "(do " (slurp name) "\n)"))))
(define (not p) (if p () (=)))


(define-macro (unless pred a b) `(if ~pred ~b ~a)))

(define (inc n) (+ n 1))
(define (dec n) (- n 1))

(prn (inc 10))
(prn (dec 10))

; (map-set! (car **scope**) (quote card) (mo (v default) `(if (list? ~v) (if (empty? ~v) ~default (car ~v)) ~default)))
(define-macro (card v default) `(if (list? ~v) (if (empty? ~v) ~default (car ~v)) ~default)))

(define-macro (assert msg v1 v2) `(do (define __v1 ~v1) (define __v2 ~v2) (if (= __v1 __v2) () (raise 'AssertionFailed {:msg ~msg :v1 '~v1 :v2 '~v2 :__v1 __v1 :__v2 __v2}))))

(assert "(card 1 99)" (card 1 99) 99)
(assert "(card () 99)" (card () 99) 99)
(assert "(card '(1 2 3) 99)" (card '(1 2 3) 99) 1)

(prn "hello world")

(define (map f l) (cond (empty? l) () :else (cons (f (car l)) (map f (cdr l)))))

(assert "map inc ()" (map inc () s) ())
(assert "map inc '(1 2 3)" (map inc '(1 2 3)) '(2 3 4))