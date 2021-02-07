(export-macro (assert-msg-equals msg v1 v2)
    `(do (define __v1 ~v1) (define __v2 ~v2) (if (= __v1 __v2) () (raise 'AssertionFailed {:msg ~msg :v1 '~v1 :v2 '~v2 :__v1 __v1 :__v2 __v2 :src *source-name*}))))

(export-macro (assert-equals v1 v2)
    `(do 
        (define __v1 ~v1) 
        (define __v2 ~v2) 
        (if (= __v1 __v2) 
            () 
            (raise 'AssertionFailed {:v1 '~v1 :v2 '~v2 :__v1 __v1 :__v2 __v2 :src *source-name*})
        )
    )
)

(export-macro (assert-signal e p)
    `(do
        (define signal-raised f)
        (try
            ~e
            (fn (signal)
                (do
                    (set! signal-raised t)
                    (~p signal)
                )
            )
        )
        (if (not signal-raised)
            (raise 'SignalExpected {:src *source-name* :e ~e})
        )
    )
)