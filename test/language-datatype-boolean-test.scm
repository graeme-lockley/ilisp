(import "../lib/unit.scm" :as Unit)

(Unit.test "boolean?"
    (Unit.assert-falsy (boolean? ()))
    (Unit.assert-truthy (boolean? #t))
    (Unit.assert-truthy (boolean? #f))
    (Unit.assert-falsy (boolean? #\A))
    (Unit.assert-falsy (boolean? 1))
    (Unit.assert-falsy (boolean? "name"))
    (Unit.assert-falsy (boolean? 'name))
    (Unit.assert-falsy (boolean? :name))
    (Unit.assert-falsy (boolean? '(1 2 3)))
    (Unit.assert-falsy (boolean? []))
    (Unit.assert-falsy (boolean? [1 2 3]))
    (Unit.assert-falsy (boolean? {}))
    (Unit.assert-falsy (boolean? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (boolean? (fn (n) (+ n 1))))
    (Unit.assert-falsy (boolean? car))
    (Unit.assert-falsy (boolean? and))
)

(Unit.test "printer"
    (Unit.assert-equals (str #f) "#f")
)