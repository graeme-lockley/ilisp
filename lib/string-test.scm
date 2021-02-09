(import "./string.scm" :as S)

(import "./unit.scm" :as Unit)

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