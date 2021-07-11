(import "../unit.scm" :as Unit)

(import "./compiler.scm" :as Compiler)
(import "./llvm/ir/module.scm" :as Module)
(import "./parser.scm" :as Parser)
(import "./scanner.scm" :as Scanner)
(import "./static.scm" :as Static)

(const (fred x)
    ;; (println (pr-str x))
    x
)

(const (compile-and-run program)
    (const ir ((|> Scanner.string->scanner Parser.scanner->ast Static.ast->tst fred Compiler.tst->ir) program))
    
    (write-file ir "test.ll")
    (Compiler.assemble "test.ll" "test.bc")
    (Compiler.run "test.bc")
)

(const (write-file ir name)
    (*builtin*.write-file name (Module.module->string ir))
)

(Unit.test "top-level - const value"
    (Unit.assert-equals (compile-and-run "(const x (+ 5 7 (- 6 8))) (const y #t) (const z \"hello\") (print x y z)") "10#thello")
)

(Unit.test "top-level - const procedure"
    (Unit.assert-equals (compile-and-run "
        (const (f a b)
            (+ a b)
        )

        (print (f 1 2))
    ") "3")
)

(Unit.test "nested const value"
    (Unit.assert-equals (compile-and-run "
        (const (f a b)
            (const sum (+ a b))

            (+ sum sum)
        )
        
        (print (f 1 2))
    ") "6")
)

(Unit.test "nested const forward value"
    (Unit.assert-equals (compile-and-run "
        (const a 9)

        (const (f)
            (print a)
            (const a 10)
            (print a)
        )

        (f)
    ") "()10")
)

(Unit.test "nested const procedure without free variables"
    (Unit.assert-equals (compile-and-run "
        (const (f a b)
            (const sum (+ a b))

            (const (g x) 
                (+ x x)
            ) 
            
            (g sum)
        )
        
        (print (f 1 2))
    ") "6")
)

(Unit.test "nested const procedure without free variables and name clash"
    (Unit.assert-equals (compile-and-run "
        (const (g x)
            (- 0 x)
        )

        (const (f a b)
            (const sum (+ a b))

            (const (g x)
                (+ x x)
            )

            (g sum)
        )
        
        (print (f 1 2))
    ") "6")
)

(Unit.test "nested const procedure without free variables and name clash with a forward declaration"
    (Unit.assert-equals (compile-and-run "
        (const (g x)
            (- 0 x)
        )

        (const (f a b)
            (const sum (+ a b))

            (const v (g sum))

            (const (g x)
                (+ x x)
            )

            v
        )
        
        (print (f 1 2))
    ") "6")
)

;; (Unit.test "nested const procedure with free variables"
;;     (Unit.assert-equals (compile-and-run "
;;         (const (f a b)
;;             (const sum (+ a b))

;;             (const (g x) 
;;                 (const sum2 (+ a b sum))

;;                 (+ sum2 x)
;;             ) 
            
;;             (g sum)
;;         )
        
;;         (print (f 1 2))
;;     ") "9")
;; )

;; (Unit.test "max procedure"
;;     (Unit.assert-equals (compile-and-run "
;;         (const (max a b)
;;             (if (< a b) b a)
;;         )

        
;;         (print (max 1 2))
;;     ") "2")
;; )

;; (Unit.test "nested nested const value"
;;     (Unit.assert-equals (compile-and-run "
;;         (const (nested a)
;;             (const a' (+ a 1))

;;             (const (nested-nested b)
;;                 (const b' (+ b 1))

;;                 (const (nested-nested-nested c)
;;                     (const c' (+ c 1))

;;                     (+ a b c a' b' c')
;;                 )

;;                 (nested-nested-nested b')
;;             )

;;             (nested-nested a')
;;         )

        
;;         (print (nested 1))
;;     ") "15")
;; )

(Unit.test "hello world"
    (Unit.assert-equals (compile-and-run "(println \"hello world\")") "hello world\n")
    (Unit.assert-equals (compile-and-run "(println \"hello\" \" \" \"world\")") "hello world\n")
    (Unit.assert-equals (compile-and-run "(println \"hello\" \" \" \"world\") (println \"bye bye love\")") "hello world\nbye bye love\n")
)

(Unit.test "literal - ()"
    (Unit.assert-equals (compile-and-run "(print ())") "()")
)

(Unit.test "literal - integer"
    (Unit.assert-equals (compile-and-run "(print 123 \" \" -456)") "123 -456")
)

(Unit.test "literal - boolean"
    (Unit.assert-equals (compile-and-run "(print #t \" \" #f)") "#t #f")
)

(Unit.test "operator +"
    (Unit.assert-equals 
        (compile-and-run "(print (+) \" \" (+ 1) \" \" (+ 1 2) \" \" (+ 1 2 3 4 5 6))")
        "0 1 3 21"
    )
)

(Unit.test "operator -"
    (Unit.assert-equals 
        (compile-and-run "(print (-) \" \" (- 1) \" \" (- 1 2) \" \" (- 1 2 3 4 5 6))")
        "0 -1 -1 -19"
    )
)

(Unit.test "operator *"
    (Unit.assert-equals 
        (compile-and-run "(print (*) \" \" (* 1) \" \" (* 1 2) \" \" (* 1 2 3 4 5 6))")
        "1 1 2 720"
    )
)

(Unit.test "operator /"
    (Unit.assert-equals 
        (compile-and-run "(print (/) \" \" (/ 1) \" \" (/ 4 2) \" \" (/ 500 50 2))")
        "1 1 2 5"
    )
)

(Unit.test "operator ="
    (Unit.assert-equals 
        (compile-and-run "(print (= 1 1) \" \" (= 1 0) \" \" (= #t #t) \" \" (= #t #f))")
        "#t #f #t #f"
    )
)

(Unit.test "operator <"
    (Unit.assert-equals 
        (compile-and-run "(print (< 1 1) \" \" (< 1 0) \" \" (< 0 1) \" \" (< #t #t) \" \" (< #t #f) \" \" (< #f #t))")
        "#f #f #t #f #f #t"
    )
)

(Unit.test "expression - if"
    (Unit.assert-equals (compile-and-run "
        (print (if) (if 1) (if (= 4 4) 1 2) (if (= 4 7) 1 2) (if (= 4 7) 1 (= 3 2) 2 3) (if (= 4 7) 1 (= 3 2) 2))
    ") "()1123()")
)

(Unit.test "function - pair"
    (Unit.assert-equals (compile-and-run "
        (print (pair 1 2))
    ") "(1 . 2)")
)

(Unit.test "function - car"
    (Unit.assert-equals (compile-and-run "
        (print (car (pair 1 (pair 2 ()))))
    ") "1")
)

(Unit.test "function - cdr"
    (Unit.assert-equals (compile-and-run "
        (print (cdr (pair 1 (pair 2 ()))))
    ") "(2)")
)

(Unit.test "function - null?"
    (Unit.assert-equals (compile-and-run "
        (print (null? ()) (null? #t) (null? 1) (null? \"hello\") (null? (pair 1 2)))
    ") "#t#f#f#f#f")
)

(Unit.test "function - boolean?"
    (Unit.assert-equals (compile-and-run "
        (print (boolean? ()) (boolean? #t) (boolean? 1) (boolean? \"hello\") (boolean? (pair 1 2)))
    ") "#f#t#f#f#f")
)

(Unit.test "function - integer?"
    (Unit.assert-equals (compile-and-run "
        (print (integer? ()) (integer? #t) (integer? 1) (integer? \"hello\") (integer? (pair 1 2)))
    ") "#f#f#t#f#f")
)

(Unit.test "function - string?"
    (Unit.assert-equals (compile-and-run "
        (print (string? ()) (string? #t) (string? 1) (string? \"hello\") (string? (pair 1 2)))
    ") "#f#f#f#t#f")
)

(Unit.test "function - pair?"
    (Unit.assert-equals (compile-and-run "
        (print (pair? ()) (pair? #t) (pair? 1) (pair? \"hello\") (pair? (pair 1 2)))
    ") "#f#f#f#f#t")
)

