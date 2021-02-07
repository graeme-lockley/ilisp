(define *tests-total* 0)
(define *tests-passed* 0)
(define *asserts-total* 0)
(define *asserts-passed* 0)

(export (inc-tests-total) (set! *tests-total* (+ *tests-total* 1)))
(export (inc-tests-passed) (set! *tests-passed* (+ *tests-passed* 1)))

(export (inc-asserts-total) (set! *asserts-total* (+ *asserts-total* 1)))
(export (inc-asserts-passed) (set! *asserts-passed* (+ *asserts-passed* 1)))

(export-macro (assert-msg-equals msg v1 v2)
    `(do 
        ((get (package-import "./unit.scm") 'inc-asserts-total))
        (define __v1 ~v1) 
        (define __v2 ~v2) 
        (if (= __v1 __v2) 
            ((get (package-import "./unit.scm") 'inc-asserts-passed))
            (raise 'AssertionFailed {:msg ~msg :v1 '~v1 :v2 '~v2 :__v1 __v1 :__v2 __v2 :src *source-name*}))
        )
)

(export-macro (assert-equals v1 v2)
    `(do 
        ((get (package-import "./unit.scm") 'inc-asserts-total))
        (define __v1 ~v1) 
        (define __v2 ~v2) 
        (if (= __v1 __v2) 
            ((get (package-import "./unit.scm") 'inc-asserts-passed))
            (raise 'AssertionFailed {:v1 '~v1 :v2 '~v2 :__v1 __v1 :__v2 __v2 :src *source-name*})
        )
    )
)

(export-macro (assert-signal e p)
    `(do
        ((get (package-import "./unit.scm") 'inc-asserts-total))
        (define signal-raised f)
        (try
            ~e
            (fn (signal)
                (do
                    (set! signal-raised t)
                    (~p signal)
                    ((get (package-import "./unit.scm") 'inc-asserts-passed))
                )
            )
        )
        (if (not signal-raised)
            (raise 'SignalExpected {:src *source-name* :e ~e})
        )
    )
)

(export-macro (test name . tests)
    `(do
        (try
            (do 
                ((get (package-import "./unit.scm") 'inc-tests-total))
                ~@tests
                ((get (package-import "./unit.scm") 'inc-tests-passed))
            )
            (fn (e)
                (raise (car e) (assoc (cdr e) :src *source-name* :test-name ~name))
            )
        )
    )
)

(export (stats)
    {
        :tests-total *tests-total*
        :tests-passed *tests-passed*
        :asserts-total *asserts-total*
        :asserts-passed *asserts-passed*
    }
)