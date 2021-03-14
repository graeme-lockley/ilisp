(import "./list.scm" :as List)
(import "./number.scm" :as Number)
(import "./sequence.scm" :as Sequence)
(import "./string-kata.scm" :as SK)
(import "./unit.scm" :as Unit)


(define integer-in-range
    (proc (min max)
        (define range (+ (- max min) 1))

        (+ min (Number.modulo (Number.abs (random)) range))
    )
)

(define (gen:integer . range)
    (if (empty? range) 
            (proc () (random))

        (= (count range) 2) 
            (proc () (integer-in-range (car range) (car (cdr range))))

        (raise 'ExpectedArgumentCount {:procedure 'gen:integer :min-arg-count: 0 :max-arg-count 2 :arguments range :usage "(gen:integer [min max])"})
    )
)

(define (gen:one-of seq)
    (proc () 
        (nth seq (integer-in-range 0 (- (count seq) 1)))
    )
)

(define (gen:tuple . gens)
    (proc () 
        (map gens (proc (g) (g)))
    )
)

(define (gen:list-of g . options)
    (do
        (define (list-of min max)
            (proc ()
                (define length (integer-in-range min max))

                (define (gen-list n)
                    (if (<= n 0) ()
                        (cons (g) (gen-list (- n 1)))
                    )
                )

                (gen-list length)
            )
        )

        (define number-of-options (count options))

        (if (= 0 number-of-options) (list-of 0 10)
            (= 1 number-of-options) (list-of (car options) 10)
            (= 2 number-of-options) (list-of (car options) (nth options 1))
            (raise 'ExpectedArgumentCount {:procedure 'gen:list-of :min-arg-count: 1 :max-arg-count 3 :arguments (cons g opitions) :usage "(gen:list-of gen [min [max]])"})
        )
    )
)

(define (gen:filter g predicate)
    (proc ()
        (define value (g))

        (if (predicate value)
            value
            ((gen:filter g predicate))
        )
    )
)

(define (gen:map g f)
    (proc () (f (g)))
)

(define (loop-n n thunk)
    (if (> n 0)
        (do (thunk)
            (loop-n (- n 1) thunk)
        )
    )
)

(define (sample gen)
    (do (define (ntimes n)
            (if (= 0 n) ()
                (cons (gen) (ntimes (- n 1)))
            )
        )

        (ntimes 10)
    )
)

(define (gen:for-all gen test)
    (loop-n 10 
        (proc () 
            (define arguments (gen))

            (try
                (test arguments)
                (proc (e)
                    (if (and (pair? e) (map? (cdr e)))
                            (raise (car e) (assoc (cdr e) :gen-arguments arguments))
                        (raise e {:gen-arguments arguments})
                    )
                )
            )
        )
    )
)

(define (gen:for-each gen test)
    (loop-n 10
        (proc ()
            (define arguments (gen))

            (try
                (apply test arguments)
                (proc (e)
                    (if (and (pair? e) (map? (cdr e)))
                            (raise (car e) (assoc (cdr e) :gen-arguments arguments))
                        (raise e {:gen-arguments arguments})
                    )
                )
            )
        )
    )   
)

(define *POSITIVE-INTEGER* (gen:integer 0 2000))
(define *INTEGER* (gen:integer (- 2000) 2000))

(define *SEPARATOR* 
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
        char->string
    )
)
(define *STRING-SEPARATOR*
    (gen:map (gen:list-of *SEPARATOR* 1) (proc (seps) (apply str seps)))
)


(define (test-sum ns)
    (Sequence.sum (filter ns (proc (n) (<= n 1000))))
)

(Unit.test "given positive integers separated with a comma or newline should return the sum of all less than 1001"
    (gen:for-all (gen:list-of *POSITIVE-INTEGER*) 
        (proc (ns)
            (Unit.assert-equals (SK.add (apply str (List.separate ns (gen:one-of '("," "\n"))))) (test-sum ns))
        )
    )
)

(Unit.test "given positive integers separated with a single character custom separator should return the sum of all less than 1001"
    (gen:for-each (gen:tuple (gen:list-of *POSITIVE-INTEGER*) *SEPARATOR*) 
        (proc (ns sep)
            (define input (str "//" sep "\n" (apply str (List.separate ns sep))))
            (Unit.assert-equals (SK.add input) (test-sum ns))
        )
    )
)

(Unit.test "given positive integers separated with multiple multi-character custom separators should return the sum of all less than 1001"
    (gen:for-each (gen:tuple (gen:list-of *POSITIVE-INTEGER*) (gen:list-of *STRING-SEPARATOR* 1)) 
        (proc (ns seps)
            (define input (str "//[" (apply str (List.separate seps "][")) "]\n" (apply str (List.separate ns (gen:one-of seps)))))
            (Unit.assert-equals (SK.add input) (test-sum ns))
        )
    )
)

(Unit.test "given integers with at least one negative should report an error with all of the negatives"
    (gen:for-all (gen:filter (gen:list-of *INTEGER*) (proc (ns) (any ns Number.negative?))) 
        (proc (ns)
            (Unit.assert-signal (SK.add (apply str (List.separate ns ",")))
                (proc (n) (Unit.assert-equals n (filter ns Number.negative?)))
            )
        )
    )
)
