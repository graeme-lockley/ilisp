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
        ((get (car **root**) :unit 'inc-asserts-total))
        (define __v1 ~v1) 
        (define __v2 ~v2) 
        (if (= __v1 __v2) 
            ((get (car **root**) :unit 'inc-asserts-passed))
            (raise 'AssertionFailed {:msg ~msg :v1 '~v1 :v2 '~v2 :__v1 __v1 :__v2 __v2 :src *source-name*}))
        )
)

(export-macro (assert-equals v1 v2)
    `(do 
        ((get (car **root**) :unit 'inc-asserts-total))
        (define __v1 ~v1) 
        (define __v2 ~v2) 
        (if (= __v1 __v2) 
            ((get (car **root**) :unit 'inc-asserts-passed))
            (raise 'AssertionFailed {:v1 '~v1 :v2 '~v2 :__v1 __v1 :__v2 __v2 :src *source-name*})
        )
    )
)

(export-macro (assert-truthy v)
    `(do 
        ((get (car **root**) :unit 'inc-asserts-total))
        (define __v ~v) 
        (if __v 
            ((get (car **root**) :unit 'inc-asserts-passed))
            (raise 'AssertionFailed {:v '~v :__v __v :src *source-name*})
        )
    )
)

(export-macro (assert-falsy v)
    `(do 
        ((get (car **root**) :unit 'inc-asserts-total))
        (define __v ~v) 
        (if __v 
            (raise 'AssertionFailed {:v '~v :__v __v :src *source-name*})
            ((get (car **root**) :unit 'inc-asserts-passed))
        )
    )
)

(export-macro (assert-signal e p)
    `(do
        ((get (car **root**) :unit 'inc-asserts-total))
        (define signal-raised #f)
        (try
            ~e
            (fn (signal)
                (do
                    (set! signal-raised #t)
                    (~p signal)
                    ((get (car **root**) :unit 'inc-asserts-passed))
                )
            )
        )
        (if (not signal-raised)
            (raise 'SignalExpected {:src *source-name* :e ~e})
        )
    )
)

(export-macro (assert-signal-name e n)
    `(Unit.assert-signal ~e (fn (s) (Unit.assert-equals (car s) ~n)))
)

(export-macro (test name . tests)
    `(do
        (try
            (do 
                (println "- " ~name)
                ((get (car **root**) :unit 'inc-tests-total))
                ~@tests
                ((get (car **root**) :unit 'inc-tests-passed))
            )
            (fn (e)
                (do
                    (if (and (pair? e) (map? (cdr e)))
                            (raise (car e) (assoc (cdr e) :src *source-name* :test-name ~name))
                        (raise e {:src *source-name* :test-name ~name})
                    )
                )
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