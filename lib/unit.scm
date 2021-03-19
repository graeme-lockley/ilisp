(const- *tests-total* 0)
(const- *tests-passed* 0)
(const- *asserts-total* 0)
(const- *asserts-passed* 0)

(const (inc-tests-total) (set! *tests-total* (+ *tests-total* 1)))
(const (inc-tests-passed) (set! *tests-passed* (+ *tests-passed* 1)))

(const (inc-asserts-total) (set! *asserts-total* (+ *asserts-total* 1)))
(const (inc-asserts-passed) (set! *asserts-passed* (+ *asserts-passed* 1)))

(export-macro (assert-msg-equals msg v1 v2)
    `((proc () 
        ((get (car **root**) :unit 'inc-asserts-total))
        (const __v1 ~v1) 
        (const __v2 ~v2) 
        (if (= __v1 __v2) 
            ((get (car **root**) :unit 'inc-asserts-passed))
            (raise 'AssertionFailed {:msg ~msg :v1 '~v1 :v2 '~v2 :__v1 __v1 :__v2 __v2 :src *source-name*})
        )
    ))
)

(export-macro (assert-equals v1 v2)
    `((proc ()
        ((get (car **root**) :unit 'inc-asserts-total))
        (const __v1 ~v1) 
        (const __v2 ~v2) 
        (if (= __v1 __v2) 
            ((get (car **root**) :unit 'inc-asserts-passed))
            (raise 'AssertionFailed {:v1 '~v1 :v2 '~v2 :__v1 __v1 :__v2 __v2 :src *source-name*})
        )
    ))
)

(export-macro (assert-truthy v)
    `((proc () 
        ((get (car **root**) :unit 'inc-asserts-total))
        (const __v ~v) 
        (if __v 
            ((get (car **root**) :unit 'inc-asserts-passed))
            (raise 'AssertionFailed {:v '~v :__v __v :src *source-name*})
        )
    ))
)

(export-macro (assert-falsy v)
    `((proc () 
        ((get (car **root**) :unit 'inc-asserts-total))
        (const __v ~v) 
        (if __v 
            (raise 'AssertionFailed {:v '~v :__v __v :src *source-name*})
            ((get (car **root**) :unit 'inc-asserts-passed))
        )
    ))
)

(export-macro (assert-signal e p)
    `((proc ()
        ((get (car **root**) :unit 'inc-asserts-total))
        (const signal-raised (*builtin*.atom #f))
        (try
            ~e
            (proc (signal)
                (do
                    (*builtin*.atom-swap! signal-raised (proc (v) #t))
                    (~p signal)
                    ((get (car **root**) :unit 'inc-asserts-passed))
                )
            )
        )
        (if (not (*builtin*.atom-dereference signal-raised))
            (raise 'SignalExpected {:src *source-name* :e ~e})
        )
    ))
)

(export-macro (assert-signal-name e n)
    `(Unit.assert-signal ~e (proc (s) (Unit.assert-equals (car s) ~n)))
)

(export-macro (test name . tests)
    `(try
        (do 
            (println "- " ~name)
            ((get (car **root**) :unit 'inc-tests-total))
            ((proc () 
                ~@tests
            ))
            ((get (car **root**) :unit 'inc-tests-passed))
        )
        (proc (e)
            (do
                (if (and (pair? e) (map? (cdr e)))
                        (raise (car e) (assoc (cdr e) :src *source-name* :test-name ~name))
                    (raise e {:src *source-name* :test-name ~name})
                )
            )
        )
    )
)

(const (stats)
    {
        :tests-total *tests-total*
        :tests-passed *tests-passed*
        :asserts-total *asserts-total*
        :asserts-passed *asserts-passed*
    }
)