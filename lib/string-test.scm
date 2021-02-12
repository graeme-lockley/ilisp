(import "./string.scm" :as S)

(import "./unit.scm" :as Unit)

(Unit.test "car"
    (Unit.assert-equals (car "h") 104)
    (Unit.assert-equals (car "hello") 104)

    (Unit.assert-signal (car "") (fn (signal) (do
        (Unit.assert-equals (car signal) 'InvalidArgument)
        (Unit.assert-equals (get (cdr signal) :arg-number) 0)
        (Unit.assert-equals (get (cdr signal) :procedure) 'car)
    )))
)

(Unit.test "cdr"
    (Unit.assert-equals (cdr "h") "")
    (Unit.assert-equals (cdr "hello") "ello")

    (Unit.assert-signal (cdr "") (fn (signal) (do
        (Unit.assert-equals (car signal) 'InvalidArgument)
        (Unit.assert-equals (get (cdr signal) :arg-number) 0)
        (Unit.assert-equals (get (cdr signal) :procedure) 'cdr)
    )))
)

(Unit.test "count"
    (Unit.assert-equals (S.count "") 0)
    (Unit.assert-equals (S.count "hello") 5)
)

(Unit.test "drop"
    (Unit.assert-equals (S.drop "" 2) "")
    (Unit.assert-equals (S.drop "hello" 2) "llo")
    (Unit.assert-equals (S.drop "hello" (- 3)) "hello")
    (Unit.assert-equals (S.drop "hello" 20) "")
)

(Unit.test "drop-right"
    (Unit.assert-equals (S.drop-right "" 2) "")
    (Unit.assert-equals (S.drop-right "hello" 2) "hel")
    (Unit.assert-equals (S.drop-right "hello" (- 3)) "hello")
    (Unit.assert-equals (S.drop-right "hello" 20) "")
)

(Unit.test "ends-with"
    (Unit.assert-equals (S.ends-with "" "") t)
    (Unit.assert-equals (S.ends-with "hello" "") t)
    (Unit.assert-equals (S.ends-with "hello" "lo") t)
    (Unit.assert-equals (S.ends-with "hello" "hello") t)

    (Unit.assert-equals (S.ends-with "hello" "Lo") f)
    (Unit.assert-equals (S.ends-with "hello" "ohello") f)

    (Unit.assert-signal (S.ends-with 1 "abv") (fn (signal) (do
        (Unit.assert-equals (car signal) 'InvalidArgument)
        (Unit.assert-equals (get (cdr signal) :arg-number) 0)
        (Unit.assert-equals (get (cdr signal) :procedure) 'string-ends-with)
    )))

    (Unit.assert-signal (S.ends-with "asd" 2) (fn (signal) (do
        (Unit.assert-equals (car signal) 'InvalidArgument)
        (Unit.assert-equals (get (cdr signal) :arg-number) 1)
        (Unit.assert-equals (get (cdr signal) :procedure) 'string-ends-with)
    )))
)

(Unit.test "filter"
    (Unit.assert-equals (S.filter "hello to the world" (fn (c) (not (< c 32)))) "hello to the world")
    (Unit.assert-equals (S.filter "hello to the world" (fn (c) (not (= c 32)))) "hellototheworld")
    (Unit.assert-equals (S.filter "hello to the world" (fn (c) (> c 110))) "ototwor")
    (Unit.assert-equals (S.filter "hello to the world" (fn (c) (> c 255))) "")
)

(Unit.test "first"
    (Unit.assert-equals (first "") ())
    (Unit.assert-equals (first "h") 104)
    (Unit.assert-equals (first "hello") 104)
)

(Unit.test "nth"
    (Unit.assert-equals (S.nth "hello" 1) 101)
    (Unit.assert-equals (S.nth "hello" 2) 108)
    (Unit.assert-equals (S.nth "hello" (- 1)) ())
    (Unit.assert-equals (S.nth "hello" 10) ())
    (Unit.assert-equals (S.nth "" 0) ())
)

(Unit.test "rest"
    (Unit.assert-equals (rest "") ())
    (Unit.assert-equals (rest "h") "")
    (Unit.assert-equals (rest "hello") "ello")
)

(Unit.test "slice"
    (Unit.assert-equals (S.slice "hello" 1 1) "e")
    (Unit.assert-equals (S.slice "hello" 1 3) "ell")
    (Unit.assert-equals (S.slice "hello" 1 10) "ello")
    (Unit.assert-equals (S.slice "hello" 0 2) "hel")
    (Unit.assert-equals (S.slice "hello" (- 10) 2) "hel")
)

(Unit.test "starts-with"
    (Unit.assert-equals (S.starts-with "" "") t)
    (Unit.assert-equals (S.starts-with "hello world" "") t)
    (Unit.assert-equals (S.starts-with "hello world" "hello") t)
    (Unit.assert-equals (S.starts-with "hello world" "hello world") t)

    (Unit.assert-equals (S.starts-with "hello world" "Hello") f)
    (Unit.assert-equals (S.starts-with "hello world" "hello worlds") f)
    (Unit.assert-equals (S.starts-with "" "h") f)

    (Unit.assert-signal (S.starts-with 1 "abv") (fn (signal) (do
        (Unit.assert-equals (car signal) 'InvalidArgument)
        (Unit.assert-equals (get (cdr signal) :arg-number) 0)
        (Unit.assert-equals (get (cdr signal) :procedure) 'string-starts-with)
    )))

    (Unit.assert-signal (S.starts-with "asd" 2) (fn (signal) (do
        (Unit.assert-equals (car signal) 'InvalidArgument)
        (Unit.assert-equals (get (cdr signal) :arg-number) 1)
        (Unit.assert-equals (get (cdr signal) :procedure) 'string-starts-with)
    )))
)

(Unit.test "take"
    (Unit.assert-equals (S.take "hello" (- 3)) "")
    (Unit.assert-equals (S.take "hello" 0) "")
    (Unit.assert-equals (S.take "hello" 2) "he")
    (Unit.assert-equals (S.take "hello" 10) "hello")
)

(Unit.test "take-right"
    (Unit.assert-equals (S.take-right "hello" (- 3)) "")
    (Unit.assert-equals (S.take-right "hello" 0) "")
    (Unit.assert-equals (S.take-right "hello" 2) "lo")
    (Unit.assert-equals (S.take-right "hello" 10) "hello")
)
