(import "../lib/unit.scm" :as Unit)

(Unit.test "thunk proc"
    (define s (*builtin*.atom 0))

    (define t (proc () (*builtin*.atom-dereference s)))

    (Unit.assert-equals (t) 0)

    (*builtin*.atom-swap! s (proc (n) (+ n 1)))

    (Unit.assert-equals (t) 1)
)

(Unit.test "identity proc"
    (Unit.assert-equals ((proc (x) x) :bob) :bob)
)

(Unit.test "proc with dot notation"
    (Unit.assert-equals ((proc (. x) x) :bob :fred) '(:bob :fred))
)

(Unit.test "proc without an body signal"
    (Unit.assert-signal-name (proc (. x)) 'InvalidArgument)
)

(Unit.test "proc with rest parameters"
    (define p (proc (a b . x) (list a b x)))

    (Unit.assert-equals (p :bob :fred) '(:bob :fred ()))
    (Unit.assert-equals (p :bob :fred :bill :pete) '(:bob :fred (:bill :pete)))
)

(Unit.test "proc has implied do"
    (define inc 
        (proc (atom)
            (*builtin*.atom-swap! atom (proc (n) (+ n 1)))
            atom
        )
    )

    (Unit.assert-equals (*builtin*.atom-dereference (inc (inc (*builtin*.atom 0)))) 2)
)
