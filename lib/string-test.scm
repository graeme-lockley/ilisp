(import "./string.scm" :as S)

(import "./unit.scm" :as Unit)

(Unit.test "any"
    (const (p c) (= c #\0))

    (Unit.assert-equals (S.any "" p) #f)
    (Unit.assert-equals (S.any "hello" p) #f)
    (Unit.assert-equals (S.any "0123" p) #t)
    (Unit.assert-equals (S.any "101" p) #t)
    (Unit.assert-equals (S.any "210" p) #t)
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
    (Unit.assert-equals (S.ends-with "" "") #t)
    (Unit.assert-equals (S.ends-with "hello" "") #t)
    (Unit.assert-equals (S.ends-with "hello" "lo") #t)
    (Unit.assert-equals (S.ends-with "hello" "hello") #t)

    (Unit.assert-equals (S.ends-with "hello" "Lo") #f)
    (Unit.assert-equals (S.ends-with "hello" "ohello") #f)

    (Unit.assert-signal (S.ends-with 1 "abv") (proc (signal) (do
        (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
        (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
        (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) (*builtin*.symbol "*builtin*.string-ends-with"))
    )))

    (Unit.assert-signal (S.ends-with "asd" 2) (proc (signal) (do
        (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
        (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 1)
        (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) (*builtin*.symbol "*builtin*.string-ends-with"))
    )))
)

(Unit.test "filter"
    (Unit.assert-equals (S.filter "hello to the world" (proc (c) (not (< c 32)))) "hello to the world")
    (Unit.assert-equals (S.filter "hello to the world" (proc (c) (not (= c 32)))) "hellototheworld")
    (Unit.assert-equals (S.filter "hello to the world" (proc (c) (> c 110))) "ototwor")
    (Unit.assert-equals (S.filter "hello to the world" (proc (c) (> c 255))) "")
)

(Unit.test "first"
    (Unit.assert-equals (first "") ())
    (Unit.assert-equals (first "h") #\h)
    (Unit.assert-equals (first "hello") #\h)
)

(Unit.test "fold"
    (const (join a b) (str (*builtin*.character->string b) a))

    (Unit.assert-equals (S.fold "" "" join) "")
    (Unit.assert-equals (S.fold "h" "" join) "h")
    (Unit.assert-equals (S.fold "hello" "" join) "olleh")

   (Unit.assert-equals (S.fold "hello" "0" (proc (a v) (str "(" a " + " (*builtin*.character->string v) ")"))) "(((((0 + h) + e) + l) + l) + o)")
)

(Unit.test "fold-right"
   (const (join a b) (str (*builtin*.character->string a) b))

   (Unit.assert-equals (S.fold-right "" "" join) "")
   (Unit.assert-equals (S.fold-right "h" "" join) "h")
   (Unit.assert-equals (S.fold-right "hello" "" join) "hello")

   (Unit.assert-equals (S.fold-right "hello" "0" (proc (v a) (str "(" (*builtin*.character->string v) " + " a ")"))) "(h + (e + (l + (l + (o + 0)))))")
)

(Unit.test "interpolate-with"
   (Unit.assert-equals (S.interpolate-with () ", ") "")
   (Unit.assert-equals (S.interpolate-with '(1) ", ") "1")
   (Unit.assert-equals (S.interpolate-with '(1 2 3) ", ") "1, 2, 3")
)

(Unit.test "nth"
    (Unit.assert-equals (S.nth "hello" 1) #\e)
    (Unit.assert-equals (S.nth "hello" 2) #\l)
    (Unit.assert-equals (S.nth "hello" (- 1)) #x00)
    (Unit.assert-equals (S.nth "hello" 10) #x00) 
    (Unit.assert-equals (S.nth "" 0) #x00)
)

(Unit.test "rest"
    (Unit.assert-equals (rest "") ())
    (Unit.assert-equals (rest "h") "")
    (Unit.assert-equals (rest "hello") "ello")
)

(Unit.test "reverse"
    (Unit.assert-equals (S.reverse "") "")
    (Unit.assert-equals (S.reverse "h") "h")
    (Unit.assert-equals (S.reverse "hello") "olleh")
)

(Unit.test "slice"
    (Unit.assert-equals (S.slice "hello" 1 1) "e")
    (Unit.assert-equals (S.slice "hello" 1 3) "ell")
    (Unit.assert-equals (S.slice "hello" 1 10) "ello")
    (Unit.assert-equals (S.slice "hello" 0 2) "hel")
    (Unit.assert-equals (S.slice "hello" (- 10) 2) "hel")
)

(Unit.test "starts-with"

    (Unit.assert-signal (S.starts-with 1 "abv") (proc (signal) (do
        (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
        (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
        (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) (*builtin*.symbol "*builtin*.string-starts-with"))
    )))

    (Unit.assert-signal (S.starts-with "asd" 2) (proc (signal) (do
        (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
        (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 1)
        (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) (*builtin*.symbol "*builtin*.string-starts-with"))
    )))
)

(Unit.test "string->list"
    (Unit.assert-equals (S.string->list "") ())
    (Unit.assert-equals (S.string->list "abc") '(#\a #\b #\c))
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
