(import "../unit.scm" :as Unit)
(import "./json.scm" :as JSON)

(Unit.test "->string - string value"
    (Unit.assert-equals (JSON.->string "hello") "\"hello\"")
    (Unit.assert-equals (JSON.->string "he\"llo") "\"he\\\"llo\"")
    (Unit.assert-equals (JSON.->string "he\tllo") "\"he\\tllo\"")
)

(Unit.test "->string - integer value"
    (Unit.assert-equals (JSON.->string 123) "123")
)

(Unit.test "->string - list value"
    (Unit.assert-equals (JSON.->string '()) "[]")
    (Unit.assert-equals (JSON.->string '(1)) "[1]")
    (Unit.assert-equals (JSON.->string '(1 2 3)) "[1 2 3]")
)

(Unit.test "->string - vector value"
    (Unit.assert-equals (JSON.->string []) "[]")
    (Unit.assert-equals (JSON.->string [1]) "[1]")
    (Unit.assert-equals (JSON.->string [1 2 3]) "[1 2 3]")
)
