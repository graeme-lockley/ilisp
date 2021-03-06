(import "./unit.scm" :as Unit)

(Unit.test "fn?"
    (Unit.assert-falsy (*builtin*.proc? ()))
    (Unit.assert-falsy (*builtin*.proc? 1))
    (Unit.assert-falsy (*builtin*.proc? #\A))
    (Unit.assert-falsy (*builtin*.proc? "name"))
    (Unit.assert-falsy (*builtin*.proc? 'name))
    (Unit.assert-falsy (*builtin*.proc? :name))
    (Unit.assert-falsy (*builtin*.proc? '(1 2 3)))
    (Unit.assert-falsy (*builtin*.proc? []))
    (Unit.assert-falsy (*builtin*.proc? [1 2 3]))
    (Unit.assert-falsy (*builtin*.proc? {}))
    (Unit.assert-falsy (*builtin*.proc? {:a 1 :b 2 :c 3}))
    (Unit.assert-truthy (*builtin*.proc? (proc (n) (+ n 1))))
    (Unit.assert-truthy (*builtin*.proc? *builtin*.pair-car))
    (Unit.assert-falsy (*builtin*.proc? and))
)

(Unit.test "number?"
    (Unit.assert-falsy (number? ()))
    (Unit.assert-truthy (number? 1))
    (Unit.assert-falsy (number? #\A))
    (Unit.assert-falsy (number? "name"))
    (Unit.assert-falsy (number? 'name))
    (Unit.assert-falsy (number? :name))
    (Unit.assert-falsy (number? '(1 2 3)))
    (Unit.assert-falsy (number? []))
    (Unit.assert-falsy (number? [1 2 3]))
    (Unit.assert-falsy (number? {}))
    (Unit.assert-falsy (number? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (number? (proc (n) (+ n 1))))
    (Unit.assert-falsy (number? *builtin*.pair-car))
    (Unit.assert-falsy (number? and))
)

(Unit.test "sequential?"
    (Unit.assert-truthy (sequential? ()))
    (Unit.assert-falsy (sequential? 1))
    (Unit.assert-falsy (sequential? #\A))
    (Unit.assert-falsy (sequential? "name"))
    (Unit.assert-falsy (sequential? 'name))
    (Unit.assert-falsy (sequential? :name))
    (Unit.assert-truthy (sequential? '(1 2 3)))
    (Unit.assert-truthy (sequential? []))
    (Unit.assert-truthy (sequential? [1 2 3]))
    (Unit.assert-falsy (sequential? {}))
    (Unit.assert-falsy (sequential? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (sequential? (proc (n) (+ n 1))))
    (Unit.assert-falsy (sequential? *builtin*.pair-car))
    (Unit.assert-falsy (sequential? and))
)

(Unit.test "symbol?"
    (Unit.assert-falsy (*builtin*.symbol? ()))
    (Unit.assert-falsy (*builtin*.symbol? 1))
    (Unit.assert-falsy (*builtin*.symbol? #\A))
    (Unit.assert-falsy (*builtin*.symbol? "name"))
    (Unit.assert-truthy (*builtin*.symbol? 'name))
    (Unit.assert-falsy (*builtin*.symbol? :name))
    (Unit.assert-falsy (*builtin*.symbol? '(1 2 3)))
    (Unit.assert-falsy (*builtin*.symbol? []))
    (Unit.assert-falsy (*builtin*.symbol? [1 2 3]))
    (Unit.assert-falsy (*builtin*.symbol? {}))
    (Unit.assert-falsy (*builtin*.symbol? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.symbol? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.symbol? *builtin*.pair-car))
    (Unit.assert-falsy (*builtin*.symbol? and))
)

(Unit.test "vector?"
    (Unit.assert-falsy (*builtin*.vector? ()))
    (Unit.assert-falsy (*builtin*.vector? 1))
    (Unit.assert-falsy (*builtin*.vector? #\A))
    (Unit.assert-falsy (*builtin*.vector? "name"))
    (Unit.assert-falsy (*builtin*.vector? 'name))
    (Unit.assert-falsy (*builtin*.vector? :name))
    (Unit.assert-falsy (*builtin*.vector? '(1 2 3)))
    (Unit.assert-truthy (*builtin*.vector? []))
    (Unit.assert-truthy (*builtin*.vector? [1 2 3]))
    (Unit.assert-falsy (*builtin*.vector? {}))
    (Unit.assert-falsy (*builtin*.vector? {:a 1 :b 2 :c 3}))
    (Unit.assert-falsy (*builtin*.vector? (proc (n) (+ n 1))))
    (Unit.assert-falsy (*builtin*.vector? *builtin*.pair-car))
    (Unit.assert-falsy (*builtin*.vector? and))
)
