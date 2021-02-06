(import "./string.scm" :as S)

(import "./unit.scm" :as Unit)

(define t (=))

(Unit.assert-equals (S.ends-with "" "") t)
(Unit.assert-equals (S.ends-with "hello" "") t)
(Unit.assert-equals (S.ends-with "hello" "lo") t)
(Unit.assert-equals (S.ends-with "hello" "hello") t)

(Unit.assert-equals (S.ends-with "hello" "Lo") ())
(Unit.assert-equals (S.ends-with "hello" "ohello") ())

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


(Unit.assert-equals (S.starts-with "" "") t)
(Unit.assert-equals (S.starts-with "hello world" "") t)
(Unit.assert-equals (S.starts-with "hello world" "hello") t)
(Unit.assert-equals (S.starts-with "hello world" "hello world") t)

(Unit.assert-equals (S.starts-with "hello world" "Hello") ())
(Unit.assert-equals (S.starts-with "hello world" "hello worlds") ())
(Unit.assert-equals (S.starts-with "" "h") ())

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
