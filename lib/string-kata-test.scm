(import "./list.scm" :as List)
(import "./number.scm" :as Number)
(import "./sequence.scm" :as Sequence)
(import "./string-kata.scm" :as SK)
(import "./unit.scm" :as Unit)


(const- integer-in-range
    (proc (min max)
        (const range (+ (- max min) 1))

        (+ min (Number.modulo (Number.abs (random)) range))
    )
)

(const- (gen:integer . range)
    (if (empty? range) 
            (proc () (random))

        (= (count range) 2) 
            (proc () (integer-in-range (*builtin*.pair-car range) (*builtin*.pair-car (*builtin*.pair-cdr range))))

        (raise 'ExpectedArgumentCount {:procedure 'gen:integer :min-arg-count: 0 :max-arg-count 2 :arguments range :usage "(gen:integer [min max])"})
    )
)

(const- (gen:one-of seq)
    (proc () 
        (nth seq (integer-in-range 0 (- (count seq) 1)))
    )
)

(const- (gen:tuple . gens)
    (proc () 
        (*builtin*.list-map gens (proc (g) (g)))
    )
)

(const- (gen:list-of g . options)
    (const (list-of min max)
        (proc ()
            (const length (integer-in-range min max))

            (const (gen-list n)
                (if (<= n 0) ()
                    (*builtin*.pair (g) (gen-list (- n 1)))
                )
            )

            (gen-list length)
        )
    )

    (const number-of-options (count options))

    (if (= 0 number-of-options) (list-of 0 10)
        (= 1 number-of-options) (list-of (*builtin*.pair-car options) 10)
        (= 2 number-of-options) (list-of (*builtin*.pair-car options) (nth options 1))
        (raise 'ExpectedArgumentCount {:procedure 'gen:list-of :min-arg-count: 1 :max-arg-count 3 :arguments (*builtin*.pair g opitions) :usage "(gen:list-of gen [min [max]])"})
    )
)

(const- (gen:filter g predicate)
    (proc ()
        (const value (g))

        (if (predicate value)
            value
            ((gen:filter g predicate))
        )
    )
)

(const- (gen:map g f)
    (proc () (f (g)))
)

(const- (loop-n n thunk)
    (if (> n 0)
        (do (thunk)
            (loop-n (- n 1) thunk)
        )
    )
)

(const- (sample gen)
    (const (ntimes n)
        (if (= 0 n) ()
            (*builtin*.pair (gen) (ntimes (- n 1)))
        )
    )

    (ntimes 10)
)

(const- (gen:for-all gen test)
    (loop-n 10 
        (proc () 
            (const arguments (gen))

            (try
                (test arguments)
                (proc (e)
                    (if (and (*builtin*.pair? e) (*builtin*.map? (*builtin*.pair-cdr e)))
                            (raise (*builtin*.pair-car e) (*builtin*.map-assoc (*builtin*.pair-cdr e) :gen-arguments arguments))
                        (raise e {:gen-arguments arguments})
                    )
                )
            )
        )
    )
)

(const- (gen:for-each gen test)
    (loop-n 10
        (proc ()
            (const arguments (gen))

            (try
                (*builtin*.apply test arguments)
                (proc (e)
                    (if (and (*builtin*.pair? e) (*builtin*.map? (*builtin*.pair-cdr e)))
                            (raise (*builtin*.pair-car e) (*builtin*.map-assoc (*builtin*.pair-cdr e) :gen-arguments arguments))
                        (raise e {:gen-arguments arguments})
                    )
                )
            )
        )
    )   
)

(const- *POSITIVE-INTEGER* (gen:integer 0 2000))
(const- *INTEGER* (gen:integer (- 2000) 2000))

(const- *SEPARATOR* 
    (gen:map 
        (gen:filter 
            (gen:integer 32 36) 
            (proc (c) 
                (not 
                    (or
                        (= c 45)      ; "-"
                        (<= 48 c 57)  ; "0".."9"
                        (= c 91)      ; "["
                    )
                )
            )
        )
        (proc (i)
            (*builtin*.character->string (*builtin*.integer->character i))
        )
    )
)
(const- *STRING-SEPARATOR*
    (gen:map (gen:list-of *SEPARATOR* 1) (proc (seps) (*builtin*.apply str seps)))
)


(const- (test-sum ns)
    (Sequence.sum (filter ns (proc (n) (<= n 1000))))
)

(Unit.test "given positive integers separated with a comma or newline should return the sum of all less than 1001"
    (gen:for-all (gen:list-of *POSITIVE-INTEGER*) 
        (proc (ns)
            (Unit.assert-equals (SK.add (*builtin*.apply str (List.separate ns (gen:one-of '("," "\n"))))) (test-sum ns))
        )
    )
)

(Unit.test "given positive integers separated with a single character custom separator should return the sum of all less than 1001"
    (gen:for-each (gen:tuple (gen:list-of *POSITIVE-INTEGER*) *SEPARATOR*) 
        (proc (ns sep)
            (const input (str "//" sep "\n" (*builtin*.apply str (List.separate ns sep))))
            (Unit.assert-equals (SK.add input) (test-sum ns))
        )
    )
)

(Unit.test "given positive integers separated with multiple multi-character custom separators should return the sum of all less than 1001"
    (gen:for-each (gen:tuple (gen:list-of *POSITIVE-INTEGER*) (gen:list-of *STRING-SEPARATOR* 1)) 
        (proc (ns seps)
            (const input (str "//[" (*builtin*.apply str (List.separate seps "][")) "]\n" (*builtin*.apply str (List.separate ns (gen:one-of seps)))))
            (Unit.assert-equals (SK.add input) (test-sum ns))
        )
    )
)

(Unit.test "given integers with at least one negative should report an error with all of the negatives"
    (gen:for-all (gen:filter (gen:list-of *INTEGER*) (proc (ns) (any ns Number.negative?))) 
        (proc (ns)
            (Unit.assert-signal (SK.add (*builtin*.apply str (List.separate ns ",")))
                (proc (n) (Unit.assert-equals n (filter ns Number.negative?)))
            )
        )
    )
)
