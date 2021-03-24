(const- *tests-total* (*builtin*.atom 0))
(const- *tests-passed* (*builtin*.atom 0))
(const- *asserts-total* (*builtin*.atom 0))
(const- *asserts-passed* (*builtin*.atom 0))

(const- (inc n) (+ n 1))

(const (inc-tests-total) (*builtin*.atom-swap! *tests-total* inc))
(const (inc-tests-passed) (*builtin*.atom-swap! *tests-passed* inc))
(const (inc-asserts-total) (*builtin*.atom-swap! *asserts-total* inc))
(const (inc-asserts-passed) (*builtin*.atom-swap! *asserts-passed* inc))

(macro (assert-msg-equals msg v1 v2)
    `((proc () 
        ((map-get (car **root**) :unit 'inc-asserts-total))
        (const __v1 ~v1) 
        (const __v2 ~v2) 
        (if (= __v1 __v2) 
            ((map-get (car **root**) :unit 'inc-asserts-passed))
            (raise 'AssertionFailed {:msg ~msg :v1 '~v1 :v2 '~v2 :__v1 __v1 :__v2 __v2 :src *source-name*})
        )
    ))
)

(macro (assert-equals v1 v2)
    `((proc ()
        ((map-get (car **root**) :unit 'inc-asserts-total))
        (const __v1 ~v1) 
        (const __v2 ~v2) 
        (if (= __v1 __v2) 
            ((map-get (car **root**) :unit 'inc-asserts-passed))
            (raise 'AssertionFailed {:v1 '~v1 :v2 '~v2 :__v1 __v1 :__v2 __v2 :src *source-name*})
        )
    ))
)

(macro (assert-truthy v)
    `((proc () 
        ((map-get (car **root**) :unit 'inc-asserts-total))
        (const __v ~v) 
        (if __v 
            ((map-get (car **root**) :unit 'inc-asserts-passed))
            (raise 'AssertionFailed {:v '~v :__v __v :src *source-name*})
        )
    ))
)

(macro (assert-falsy v)
    `((proc () 
        ((map-get (car **root**) :unit 'inc-asserts-total))
        (const __v ~v) 
        (if __v 
            (raise 'AssertionFailed {:v '~v :__v __v :src *source-name*})
            ((map-get (car **root**) :unit 'inc-asserts-passed))
        )
    ))
)

(macro (assert-signal e p)
    `((proc ()
        ((map-get (car **root**) :unit 'inc-asserts-total))
        (const signal-raised (*builtin*.atom #f))
        (try
            ~e
            (proc (signal)
                (do
                    (*builtin*.atom-swap! signal-raised (proc (v) #t))
                    (~p signal)
                    ((map-get (car **root**) :unit 'inc-asserts-passed))
                )
            )
        )
        (if (not (*builtin*.atom-dereference signal-raised))
            (raise 'SignalExpected {:src *source-name* :e ~e})
        )
    ))
)

(macro (assert-signal-name e n)
    `(Unit.assert-signal ~e (proc (s) (Unit.assert-equals (car s) ~n)))
)

(macro (test name . tests)
    `(try
        (do 
            (println "- " ~name)
            ((map-get (car **root**) :unit 'inc-tests-total))
            ((proc () 
                ~@tests
            ))
            ((map-get (car **root**) :unit 'inc-tests-passed))
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
        :tests-total (*builtin*.atom-dereference *tests-total*)
        :tests-passed (*builtin*.atom-dereference *tests-passed*)
        :asserts-total (*builtin*.atom-dereference *asserts-total*)
        :asserts-passed (*builtin*.atom-dereference *asserts-passed*)
    }
)