(import "./list.scm" :as List)

(import "./unit.scm" :as Unit)

(Unit.test "count"
    (Unit.assert-equals (List.count ()) 0)
    (Unit.assert-equals (List.count '(1)) 1)
    (Unit.assert-equals (List.count '(1 1 1 1 1 1)) 6)
)

(Unit.test "drop"
    (Unit.assert-equals (List.drop () 2) '())
    (Unit.assert-equals (List.drop '(0 1 2 3 4 5 6 7) 2) '(2 3 4 5 6 7))
    (Unit.assert-equals (List.drop '(0 1 2 3 4 5 6 7) (- 3)) '(0 1 2 3 4 5 6 7))
    (Unit.assert-equals (List.drop '(0 1 2) 10) ())
)

(Unit.test "drop-right"
    (Unit.assert-equals (List.drop-right () 2) ())
    (Unit.assert-equals (List.drop-right '(0 1 2 3 4 5 6 7) 2) '(0 1 2 3 4 5))
    (Unit.assert-equals (List.drop-right '(0 1 2 3 4 5 6 7) (- 3)) '(0 1 2 3 4 5 6 7))
    (Unit.assert-equals (List.drop-right '(0 1 2) 10) ())
)

(Unit.test "ends-with"
    (Unit.assert-truthy (List.ends-with '(1 2 3) ()))
    (Unit.assert-truthy (List.ends-with '(1 2 3) '(2 3)))
    (Unit.assert-truthy (List.ends-with '(1 2 3) '(1 2 3)))

    (Unit.assert-falsy (List.ends-with '(1 2 3) '(1)))
    (Unit.assert-falsy (List.ends-with '(1 2) '(0 1 2)))
)

(Unit.test "filter"
    (const (even? n) (= (* (/ n 2) 2) n))
    (Unit.assert-equals (List.filter '() even?) '())
    (Unit.assert-equals (List.filter '(1 3 5 7 9 11) even?) '())
    (Unit.assert-equals (List.filter '(1 2 3 4 5) even?) '(2 4))
    (Unit.assert-equals (List.filter '(2 4 6 8 10 12) even?) '(2 4 6 8 10 12))

    (Unit.assert-signal (List.filter [2 4 6 8 10 12] even?) (proc (signal) (do
        (Unit.assert-equals (*builtin*.pair-car signal) 'InvalidArgument)
        (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :arg-number) 0)
        (Unit.assert-equals (map-get (*builtin*.pair-cdr signal) :procedure) (*builtin*.symbol "*builtin*.list-filter"))
    )))
)

(Unit.test "first"
    (Unit.assert-equals (first '(1)) 1)
    (Unit.assert-equals (first '(1 2 3)) 1)
    (Unit.assert-equals (first ()) ())
)

(Unit.test "fold"
    (const (icons a b) (*builtin*.pair b a))

    (Unit.assert-equals (List.fold () () icons) ())
    (Unit.assert-equals (List.fold '(1) () icons) '(1))
    (Unit.assert-equals (List.fold '(1 2 3 4 5) () icons) '(5 4 3 2 1))

   (Unit.assert-equals (List.fold '(1 2 3 4 5) "0" (proc (a v) (str "(" a " + " v ")"))) "(((((0 + 1) + 2) + 3) + 4) + 5)")
)

(Unit.test "fold-right"
   (Unit.assert-equals (List.fold-right () () *builtin*.pair) ())
   (Unit.assert-equals (List.fold-right '(1) () *builtin*.pair) '(1))
   (Unit.assert-equals (List.fold-right '(1 2 3 4 5) () *builtin*.pair) '(1 2 3 4 5))

   (Unit.assert-equals (List.fold-right '(1 2 3 4 5) "0" (proc (v a) (str "(" v " + " a ")"))) "(1 + (2 + (3 + (4 + (5 + 0)))))")
)

(Unit.test "list->string"
    (Unit.assert-equals (List.list->string ()) "")
    (Unit.assert-equals (List.list->string '(1 2 3)) "123")
    (Unit.assert-equals (List.list->string '(#\a 2 "hello" fred :bob #f (1 2 3) [a b c])) "a2hellofred:bob#f(123)[abc]")
)

(Unit.test "list?"
    (Unit.assert-truthy (List.list? ()))
    (Unit.assert-truthy (List.list? '(1)))
    (Unit.assert-truthy (List.list? '(1 2 3)))

    (Unit.assert-falsy (List.list? (pair 1 (pair 2 3))))
)

(Unit.test "nth"
    (Unit.assert-equals (List.nth () 5) ())
    (Unit.assert-equals (List.nth (list 1 2 3) (- 1)) ())
    (Unit.assert-equals (List.nth (list 1) 0) 1)
    (Unit.assert-equals (List.nth (list 1 2) 1) 2)
    (Unit.assert-equals (List.nth (list 1 2) 2) ())
    (Unit.assert-equals (List.nth (list 1 2 () 4 5) 2) ())
)

(Unit.test "rest"
    (Unit.assert-equals (rest '(1)) ())
    (Unit.assert-equals (rest '(1 2 3)) '(2 3))
    (Unit.assert-equals (rest ()) ())
)

(Unit.test "reverse"
    (Unit.assert-equals (List.reverse ()) ())
    (Unit.assert-equals (List.reverse '(1 2 3 4 5)) '(5 4 3 2 1))
)

(Unit.test "separate"
    (const (sep)
        (const c (*builtin*.atom 64))
        (const (inc n) (+ n 1))

        (proc ()
            (*builtin*.atom-swap! c inc)
            (*builtin*.character->string (*builtin*.integer->character (*builtin*.atom-dereference c)))
        )
    )

    (Unit.assert-equals (List.separate () ",") ())
    (Unit.assert-equals (List.separate '(1) ",") '(1))
    (Unit.assert-equals (List.separate '(5 4 3 2 1) ",") '(5 "," 4 "," 3 "," 2 "," 1))

    (Unit.assert-equals (List.separate () (sep)) ())
    (Unit.assert-equals (List.separate '(1) (sep)) '(1))
    (Unit.assert-equals (List.separate '(5 4 3 2 1) (sep)) '(5 "A" 4 "B" 3 "C" 2 "D" 1))
)

(Unit.test "slice"
    (Unit.assert-equals (List.slice () 2 3) ())
    (Unit.assert-equals (List.slice '(0 1 2 3 4 5 6 7) 2 5) '(2 3 4 5))
    (Unit.assert-equals (List.slice '(0 1 2 3 4 5 6 7) 6 10) '(6 7))
    (Unit.assert-equals (List.slice '(0 1 2 3 4 5 6 7) (- 10) 2) '(0 1 2))
    (Unit.assert-equals (List.slice '(0 1 2 3 4 5 6 7) 5 2) ())
)

(Unit.test "starts-with"
    (Unit.assert-truthy (List.starts-with '(1 2 3) ()))
    (Unit.assert-truthy (List.starts-with '(1 2 3) '(1 2)))
    (Unit.assert-truthy (List.starts-with '(1 2 3) '(1 2 3)))

    (Unit.assert-falsy (List.starts-with '(1 2 3) '(3)))
    (Unit.assert-falsy (List.starts-with '(1 2) '(1 2 3)))
)

(Unit.test "take"
    (Unit.assert-equals (List.take () 2) ())
    (Unit.assert-equals (List.take '(0 1 2 3 4 5 6 7) 2) '(0 1))
    (Unit.assert-equals (List.take '(0 1 2 3 4 5 6 7) (- 3)) ())
    (Unit.assert-equals (List.take '(0 1 2) 10) '(0 1 2))
)

(Unit.test "take-right"
    (Unit.assert-equals (List.take-right () 2) ())
    (Unit.assert-equals (List.take-right '(0 1 2 3 4 5 6 7) 2) '(6 7))
    (Unit.assert-equals (List.take-right '(0 1 2 3 4 5 6 7) (- 3)) ())
    (Unit.assert-equals (List.take-right '(0 1 2) 10) '(0 1 2))
)
